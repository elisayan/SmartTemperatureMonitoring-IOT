package control.unit.backed;

import io.vertx.core.AbstractVerticle;
import io.vertx.mqtt.MqttClient;

public class MQTTAgent extends AbstractVerticle {
    private static final String BROKER_ADDRESS = "test.mosquitto.org";// "broker.mqtt-dashboard.com";
    private static final String TEMPERATURE_TOPIC = "temperature/data";
    private static final double T1 = 10.0, T2 = 20.0;
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
                                System.err.println("Error at handle temperature: " + e.getMessage());
                                e.printStackTrace();
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

    public void setController(Controller controller) {
        this.controller = controller;
    }

    public void resetAlarmState() {
        this.currentState = SystemState.NORMAL;
        this.tooHotStartTime = 0;
    }

    public SystemState getCurrentState() {
        return currentState;
    }

    private SystemState updateSystemState(double temp) {
        long now = System.currentTimeMillis();
    
        switch (currentState) {
            case NORMAL:
            case HOT:
                if (temp > T2) {
                    tooHotStartTime = now;
                    return SystemState.TOO_HOT;
                } else if (temp >= T1) {
                    return SystemState.HOT;
                } else {
                    return SystemState.NORMAL;
                }
    
            case TOO_HOT:
                if (temp <= T2) {
                    return temp >= T1 ? SystemState.HOT : SystemState.NORMAL;
                } else if (now - tooHotStartTime > DT) {
                    return SystemState.ALARM;
                }
                return SystemState.TOO_HOT;
    
            case ALARM:
                return SystemState.ALARM;
        }
    
        return currentState;
    }
    
    private int calculateWindowPosition(double temp) {
        if (temp < T1)
            return 0;
        if (temp > T2)
            return 100;
        return (int) (((temp - T1) / (T2 - T1)) * 100);
    }

    private void handleTemperature(String tempStr) throws InterruptedException {
        try {
            double temp = Double.parseDouble(tempStr);
            currentState = updateSystemState(temp);
            int pos = calculateWindowPosition(temp);

            controller.updateArduinoData(temp, pos);
            controller.updateDashboardData(temp, pos, currentState.name());
        } catch (NumberFormatException e) {
            System.err.println("Invalid temperature format: '" + tempStr + "'");
            System.err.println(e);
        }
    }

    @Override
    public void stop() {
        client.disconnect();
    }
}
