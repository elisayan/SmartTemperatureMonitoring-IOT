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

    private double lastSentTemp = -1;
    private int lastSentPos = -1;

    private String mode = "AUTOMATIC";
    private int position = -1;
    
    private enum SystemState {
        NORMAL, HOT, TOO_HOT, ALARM
    }

    @Override
    public void start() {
        // startSerialListener();
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

        if (currentState == SystemState.TOO_HOT && System.currentTimeMillis() - tooHotStartTime > DT) {
            currentState = SystemState.ALARM;
        }
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
            position = calculateWindowPosition(temp);
            //dataService.addTemperatureData(temp);
            updateSystemState(temp);

            // if (!dataService.getCurrentMode().equals(mode)) {
            //     if (dataService.isModeChanged()) {
            //         mode = dataService.getCurrentMode();
            //     }
            // }
            //dataService.updateState(position, currentState.name());
            sendToArduino(position, temp);
        } catch (NumberFormatException e) {
            System.err.println("Invalid temperature format");
        }
    }

    private void sendToArduino(int pos, double temp) throws InterruptedException {
        try {
            // if (dataService.isModeChanged()) {
            //     mode = dataService.getCurrentMode();
            //     sendMode();
            //     System.out.println("Mode synchronized to DataService: " + mode);   
            // }

            // if (dataService.getCurrentMode().equals("MANUAL")) {
            //     sendPosition(dataService.getDashboardPosition());
            //     System.out.println("send dashboard position: "+dataService.getDashboardPosition());
            // } else {
            //     sendPosition(pos);
            // }
            sendTemperature(temp);
            System.out.println("Send to arduino: POS: "+pos+" TEMP: "+temp+"\n");
        } catch (Exception e) {
            System.err.println("Failed to send message to Arduino: " + e.getMessage());
        }
    }

    private void sendTemperature(double temp) {
        if (temp != lastSentTemp) {
            String msg = String.format("TEMP:%.2f\n", temp);
            //serialChannel.sendMsg(msg);
            lastSentTemp = temp;
        } else {
            System.out.println("Temperature unchanged. Skipping send to Arduino.");
        }
    }

    private void sendPosition(int pos) {
        if (pos != lastSentPos) {
            String msg = String.format("POS:%d\n", pos);
            //serialChannel.sendMsg(msg);
            lastSentPos = pos;
        } else {
            System.out.println("Position unchanged. Skipping send to Arduino.");
        }
    }

    private void sendMode() {
        try {
            String msg = String.format("MODE:%s\n", mode);
            //serialChannel.sendMsg(msg);
            System.out.println("Sent mode to Arduino: " + mode);
        } catch (Exception e) {
            System.err.println("Failed to send mode to Arduino: " + e.getMessage());
        }
    }

    // private void startSerialListener() {
    //     new Thread(() -> {
    //         while (true) {
    //             try {
    //                 if (serialChannel.isMsgAvailable()) {
    //                     String msg = serialChannel.receiveMsg();
    //                     if (msg.startsWith("MODE:")) {
    //                         mode = msg.split(":")[1].trim(); //to check maaybe error at here
    //                         System.out.println("MQTT mode: " + mode);
    //                     }
    //                 }
    //                 Thread.sleep(100);
    //             } catch (InterruptedException e) {
    //                 e.printStackTrace();
    //             }
    //         }
    //     }).start();
    // }

    @Override
    public void stop() {
        // if (serialChannel != null) {
        //     ((SerialCommChannel) serialChannel).close();
        // }
        client.disconnect();
    }
}
