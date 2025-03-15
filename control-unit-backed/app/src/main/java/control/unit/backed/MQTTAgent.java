package control.unit.backed;

import io.vertx.core.AbstractVerticle;
import io.vertx.mqtt.MqttClient;

public class MQTTAgent extends AbstractVerticle {
    private static final String BROKER_ADDRESS = "test.mosquitto.org";// "broker.mqtt-dashboard.com";
    private static final String TEMPERATURE_TOPIC = "temperature/data";
    private static final double T1 = 10.0, T2 = 25.0;
    private static final long DT = 10000;

    private enum SystemState {
        NORMAL, HOT, TOO_HOT, ALARM
    }

    private SerialCommChannel serialChannel;
    private MqttClient client;
    private DataService dataService;
    private SystemState currentState = SystemState.NORMAL;
    private long tooHotStartTime = 0;

    private double lastSentTemp = -1;
    private int lastSentPos = -1;
    private String mode = "AUTOMATIC";

    public MQTTAgent(DataService dataService, SerialCommChannel serialChannel) {
        this.dataService = dataService;
        this.serialChannel = serialChannel;
    }

    @Override
    public void start() {
        startSerialListener();
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
                        System.currentTimeMillis() - tooHotStartTime > DT) ? "ALARM" : currentState.name());
    }

    private void handleTemperature(String tempStr) throws InterruptedException {
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
        if (temp < T1)
            return 0;
        if (temp > T2)
            return 90;
        return (int) (((temp - T1) / (T2 - T1)) * 90);
    }

    private void sendToArduino(int pos, double temp) throws InterruptedException {
        try {
            if (mode.equals("AUTOMATIC") && dataService.getCurrentMode().equals("AUTOMATIC")) {
                if (temp != lastSentTemp || pos != lastSentPos) {
                    String message = String.format("POS:%d\n", pos, temp);
                    serialChannel.sendMsg(message);
                    System.out.print("Sent to Arduino: \n" + message);

                    String message1 = String.format("TEMP:%.2f\n", temp);
                    serialChannel.sendMsg(message1);
                    System.out.println(message1);

                    lastSentTemp = temp;
                    lastSentPos = pos;
                } else {
                    System.out.println("Data unchanged. Skipping send to Arduino.");
                }
            } else {
                System.out.println("System in manual mode, only update temperature");
                String message1 = String.format("TEMP:%.2f\n", temp);
                serialChannel.sendMsg(message1);
                System.out.println(message1);
            }
        } catch (Exception e) {
            System.err.println("Failed to send message to Arduino: " + e.getMessage());
        }
    }

    private void startSerialListener() {
        new Thread(() -> {
            while (true) {
                try {
                    if (serialChannel.isMsgAvailable()) {
                        String msg = serialChannel.receiveMsg();
                        if (msg.startsWith("MODE:")) {
                            mode = msg.split(":")[1].trim();
                            System.out.println(mode);
                        }
                    }
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public void stop() {
        if (serialChannel != null) {
            ((SerialCommChannel) serialChannel).close();
        }
        client.disconnect();
    }
}
