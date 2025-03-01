package control.unit.backed;

import io.vertx.core.AbstractVerticle;
import io.vertx.mqtt.MqttClient;

public class MQTTAgent extends AbstractVerticle {
    private static final String BROKER_ADDRESS = "test.mosquitto.org";
    private static final String TEMPERATURE_TOPIC = "temperature/data";
    private static final double T1 = 5.0, T2 = 15.0;
    private static final long DT = 5000;

    private enum SystemState { NORMAL, HOT, TOO_HOT, ALARM }
    private SerialCommChannel serialChannel;
    private MqttClient client;
    private DataService dataService;
    private SystemState currentState = SystemState.NORMAL;
    private long tooHotStartTime = 0;

    public MQTTAgent(DataService dataService, SerialCommChannel serialChannel) {
        this.dataService = dataService;
        this.serialChannel = serialChannel;
    }

    @Override
    public void start() {
        try {            
            client = MqttClient.create(vertx);
            client.connect(1883, BROKER_ADDRESS, c -> {
                if (c.succeeded()) {
                    client.subscribe(TEMPERATURE_TOPIC, 1);
                    
                    client.publishHandler(msg -> {
                        if (msg.topicName().equals(TEMPERATURE_TOPIC)) {
                            String payload = msg.payload().toString();
                            handleTemperature(payload);
                        }
                    });
                } else {
                    System.err.println("MQTT connection failed");
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void updateSystemState(double temp) {
        if (temp < T1) {
            currentState = SystemState.NORMAL;
        } else if (temp <= T2) {
            currentState = SystemState.HOT;
        } else {
            if (currentState != SystemState.TOO_HOT) {
                tooHotStartTime = System.currentTimeMillis();
            }
            currentState = SystemState.TOO_HOT;
        }
        
        dataService.updateState(
            calculateWindowPosition(temp),
            (currentState == SystemState.TOO_HOT && 
             System.currentTimeMillis() - tooHotStartTime > DT) ? "ALARM" : currentState.name()
        );
    }

    private void handleTemperature(String tempStr) {
        try {
            double temp = Double.parseDouble(tempStr);
            updateSystemState(temp);
            dataService.addTemperatureData(temp);
            int windowPos = calculateWindowPosition(temp);
            sendToArduino(windowPos, temp);
        } catch (NumberFormatException e) {
            System.err.println("Invalid temperature format");
        }
    }

    private int calculateWindowPosition(double temp) {
        if (temp < T1) return 0;
        if (temp > T2) return 90;
        return (int) (((temp - T1) / (T2 - T1)) * 90);
    }

    private void sendToArduino(int pos, double temp) {
        String mes1 = String.format("TEMP:%.2f \n", temp);
        String mes2 = String.format("POS:%d \n", pos);
        serialChannel.sendMsg(mes1);
        serialChannel.sendMsg(mes2);
        System.out.println(mes1);
        System.out.println(mes2);
    }

    @Override
    public void stop() {
        if (serialChannel != null) {
            ((SerialCommChannel) serialChannel).close();
        }
        client.disconnect();
    }
}