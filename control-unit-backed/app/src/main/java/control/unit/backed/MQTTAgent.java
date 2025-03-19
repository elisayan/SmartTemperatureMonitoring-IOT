package control.unit.backed;

import io.vertx.core.AbstractVerticle;
import io.vertx.mqtt.MqttClient;

public class MQTTAgent extends AbstractVerticle {
    private static final String BROKER_ADDRESS = "test.mosquitto.org";// "broker.mqtt-dashboard.com";
    private static final String TEMPERATURE_TOPIC = "temperature/data";
    private static final double T1 = 10.0, T2 = 25.0;
    private static final long DT = 5000;

    private MqttClient client;
    private Controller controller;

    private SystemState currentState = SystemState.NORMAL;
    private long tooHotStartTime = 0;
    
    public enum SystemState {
        NORMAL, HOT, TOO_HOT, ALARM
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
                            try {
                                handleTemperature(payload);
                            } catch (Exception e) {
                                System.err.println("Error at handle temperature");
                            }
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

    public void setController(Controller controller){
        this.controller = controller;
    }

    private SystemState updateSystemState(double temp) {
        if (temp > T2 && currentState != SystemState.TOO_HOT) {
            tooHotStartTime = System.currentTimeMillis();
        }
    
        return temp < T1 ? SystemState.NORMAL :
                       temp <= T2 ? SystemState.HOT :
                       System.currentTimeMillis() - tooHotStartTime > DT ? SystemState.ALARM :
                       SystemState.TOO_HOT;
    }

    private int calculateWindowPosition(double temp) {
        if (temp < T1)
            return 0;
        if (temp > T2)
            return 90;
        return (int) (((temp - T1) / (T2 - T1)) * 90);
    }

    private void handleTemperature(String tempStr) throws InterruptedException {
        try {
            double temp = Double.parseDouble(tempStr);
            int position = calculateWindowPosition(temp);
            SystemState state = updateSystemState(temp);
            controller.updateSystem(temp, position, state.name());
        } catch (NumberFormatException e) {
            System.err.println("Invalid temperature format");
        }
    }

    @Override
    public void stop() {
        client.disconnect();
    }
}
