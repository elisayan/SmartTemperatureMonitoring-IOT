package control.unit.backed;

import java.time.LocalDateTime;

public class Controller {

    private static final String PORT = "COM3";
    private static final int RATE = 115200;

    private final DataService dashboard;
    private final MQTTAgent mqttAgent;
    private final SerialCommChannel serialChannel;

    private String arduino_mode = null;
    private LocalDateTime arduinoModeLastModified = null;
    private String mode = "AUTOMATIC";

    private int arduino_pos = -1;
    private int pos = -1;

    private String source = null;

    public Controller(DataService dashboard, MQTTAgent mqttAgent) throws Exception {
        this.dashboard = dashboard;
        this.mqttAgent = mqttAgent;
        this.serialChannel = new SerialCommChannel(PORT, RATE, this);
    }

    public void receiveMsg(String msg) {
        if (msg.startsWith("MODE:")) {
            arduino_mode = msg.split(":")[1].trim();
            arduinoModeLastModified = LocalDateTime.now();
            synchronizeAndUpdateMode();
        } else if (msg.startsWith("POS:")) {
            arduino_pos = Integer.parseInt(msg.split(":")[1].trim());
        }
    }

    public void updateDashboardData(double temp, int position, String state) {
        dashboard.addTemperatureData(temp);
        dashboard.updateState(state);
        if (mode.equals("MANUAL")) {
            dashboard.updateWindow(pos);
        } else if (mode.equals("AUTOMATIC")) {
            dashboard.updateWindow(position);
        }
    }

    public void updateArduinoData(double temp, int position) throws InterruptedException {
        if (mode.equals("MANUAL")) {
            if (source.equals("ARDUINO")) {
                pos = arduino_pos;
                dashboard.updateWindow(pos);
            } else if (source.equals("DASHBOARD")) {
                pos = dashboard.getDashboardPosition();
                sendPosition(pos);
            }
            Thread.sleep(400);
            sendTemperature(temp);
            System.out.println("manual system updated-> temp: " + temp + " pos: " + pos);
        } else if (mode.equals("AUTOMATIC")) {
            pos = position;
            sendPosition(pos);
            System.out.println("automatic system updated-> pos: " + pos);
        }
    }

    public void synchronizeAndUpdateMode() {
        this.mode = synchronizeMode();
    }

    public void resolveAlarm() {
        if(mqttAgent.getCurrentState() == MQTTAgent.TemperatureState.ALARM) {
            mqttAgent.resetAlarmState();
            dashboard.updateState("NORMAL");
            System.out.println("Alarm manually resolved");
        }
    }

    private String synchronizeMode() {
        LocalDateTime dashboardTime = dashboard.getModeLastModifiedTime();
        String dashboardMode = dashboard.getCurrentMode();

        if (arduinoModeLastModified == null && dashboardTime == null) {
            return mode;
        }

        if (arduinoModeLastModified == null) {
            sendMode(dashboardMode);
            source = "DASHBOARD";
            return dashboardMode;
        }

        if (dashboardTime == null) {
            dashboard.updateMode(arduino_mode);
            source = "ARDUINO";
            return arduino_mode;
        }

        if (arduinoModeLastModified.isAfter(dashboardTime)) {
            dashboard.updateMode(arduino_mode);
            source = "ARDUINO";
            return arduino_mode;
        } else {
            sendMode(dashboardMode);
            source = "DASHBOARD";
            return dashboardMode;
        }
    }

    private void sendMode(String mode) {
        this.serialChannel.sendMsg("MODE:" + mode);
    }

    private void sendPosition(int pos) {
        this.serialChannel.sendMsg("POS:" + pos);
    }

    private void sendTemperature(double temp) {
        this.serialChannel.sendMsg("TEMP:" + temp);
    }
}
