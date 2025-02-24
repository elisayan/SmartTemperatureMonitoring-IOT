package control.unit.backed;

import io.vertx.core.AbstractVerticle;
import io.vertx.mqtt.MqttClient;

public class MQTTAgent extends AbstractVerticle {
    private static final String BROKER_ADDRESS = "broker.mqtt-dashboard.com";
    private static final String TEMPERATURE_TOPIC = "temperature/data";
    private static final double T1 = 5.0, T2 = 15.0;
    private static final long DT = 10000;

    private enum SystemState { NORMAL, HOT, TOO_HOT, ALARM }
    private CommChannel serialChannel;
    private MqttClient client;
    private DataService dataService;
    private SystemState currentState = SystemState.NORMAL;
    private long tooHotStartTime = 0;

    public MQTTAgent(DataService dataService) {
        this.dataService = dataService;
    }

    @Override
    public void start() {
        try {
            serialChannel = new SerialCommChannel("COM3", 9600);
            
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
            currentState.name(),
            calculateWindowPosition(temp),
            (currentState == SystemState.TOO_HOT && 
             System.currentTimeMillis() - tooHotStartTime > DT) ? "ALARM" : "NORMAL"
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
        String cmd = String.format("TEMP:%.2f,POS:%d\n", temp, pos);
        serialChannel.sendMsg(cmd);
        System.out.println("send temperature");
    }

    @Override
    public void stop() {
        if (serialChannel != null) {
            ((SerialCommChannel) serialChannel).close();
        }
        client.disconnect();
    }
}