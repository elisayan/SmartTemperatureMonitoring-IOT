package control.unit.backed;

import java.time.LocalDateTime;
import java.util.Objects;

public class Controller {

    private static final String PORT = "COM3";
    private static final int RATE = 115200;

    private final DataService dataService;
    private final SerialCommChannel serialChannel;

    private String arduino_mode = null;
    private LocalDateTime arduinoModeLastModified;
    private String mode = "AUTOMATIC";

    private int arduino_pos = -1;
    private LocalDateTime arduinoPosLastModified;
    private int pos = -1;

    private String source = null;

    public Controller(DataService dataService) throws Exception {
        this.dataService = dataService;
        this.serialChannel = new SerialCommChannel(PORT, RATE, this);
    }

    public void receiveMsg(String msg) {
        if (msg.startsWith("MODE:")) {
            arduino_mode = msg.split(":")[1].trim();
            arduinoModeLastModified = LocalDateTime.now();
        } else if (msg.startsWith("POS:")) {
            arduino_pos = Integer.parseInt(msg.split(":")[1].trim());
            arduinoPosLastModified = LocalDateTime.now();
        }
    }

    public void updateDashboardData(double temp, int position, String state) {
        dataService.addTemperatureData(temp);
        dataService.updateState(state);
        if (mode.equals("MANUAL")) {
            dataService.updateWindow(pos);
        } else if (mode.equals("AUTOMATIC")) {
            dataService.updateWindow(position);
        }
    }

    public void updateArduinoData(double temp, int position) {
        mode = synchronizeMode();

        if (mode.equals("MANUAL")) {
            if (source.equals("ARDUINO")) {
                pos = arduino_pos;
                dataService.updateWindow(pos);
            } else if (source.equals("DASHBOARD")){
                pos = dataService.getDashboardPosition();
            }
            sendPosition(pos, temp);
            System.out.println("manual system updated-> temp: " + temp + " pos: " + pos);
        } else if (mode.equals("AUTOMATIC")) {
            pos = position;
            sendPosition(pos, temp);
            System.out.println("automatic system updated-> pos: " + pos);
        }
    }

    private String synchronizeMode() {
        LocalDateTime serviceTime = dataService.getModeLastModifiedTime();
        String currentMode = dataService.getCurrentMode();

        if (arduinoModeLastModified == null && serviceTime == null) {
            return mode;
        }

        if (arduinoModeLastModified == null) {
            sendMode(currentMode);
            source = "DASHBOARD";
            return currentMode;
        }

        if (serviceTime == null) {
            dataService.updateMode(arduino_mode);
            source = "ARDUINO";
            return arduino_mode;
        }

        if (arduinoModeLastModified.isAfter(serviceTime)) {
            dataService.updateMode(arduino_mode);
            source = "ARDUINO";
            return arduino_mode;
        } else {
            sendMode(currentMode);
            source = "DASHBOARD";
            return currentMode;
        }
    }

    // private int synchronizePosition() {
    // LocalDateTime dashboardTime = dataService.getDashboardPosLastModifiedTime();
    // int currentDashboardPos = dataService.getDashboardPosition();

    // if (arduino_pos == currentDashboardPos) {
    // return arduino_pos;
    // }

    // if (arduinoPosLastModified == null && dashboardTime == null) {
    // return pos;
    // }

    // if (arduinoPosLastModified == null) {
    // return currentDashboardPos;
    // }

    // if (dashboardTime == null) {
    // return arduino_pos;
    // }

    // if (arduinoPosLastModified.isAfter(dashboardTime)) {
    // return arduino_pos;
    // } else {
    // return currentDashboardPos;
    // }
    // }

    private void sendMode(String mode) {
        this.serialChannel.sendMsg("MODE:" + mode);
    }

    private void sendPosition(int pos, double temp) {
        this.serialChannel.sendMsg("DATA:");
        String message = "TEMP:" + temp + " POS:" + pos;
        this.serialChannel.sendMsg(message);
    }
}
