package control.unit.backed;

import java.time.LocalDateTime;
import java.util.Objects;

public class Controller {

    private static final String PORT = "COM3";
    private static final int RATE = 115200;

    private final MQTTAgent mqtt;
    private final DataService dataService;
    private final SerialCommChannel serialChannel;

    private String arduino_mode = null;
    private LocalDateTime arduinoModeLastModified;
    private String mode = null;

    private int arduino_pos = -1;
    private LocalDateTime arduinoPosLastModified;
    private int pos = -1;

    public Controller(MQTTAgent mqtt, DataService dataService) throws Exception {
        this.mqtt = mqtt;
        this.dataService = dataService;
        this.serialChannel = new SerialCommChannel(PORT, RATE, this);

        this.mqtt.setController(this);
        this.dataService.setController(this);
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
        dataService.updateWindow(position);
        dataService.updateState(state);
    }

    public void updateArduinoData(double temp, int position) {
        mode = synchronizeMode();

        if (mode.equals("MANUAL")) {
            sendTemperature(temp);
            pos = arduino_pos;
            System.out.println("manual system updated-> temp: " + temp + " pos: " + pos);
        } else if (mode.equals("AUTOMATIC")) {
            sendPosition(position);
            System.out.println("automatic system updated-> pos: " + position);
        }
    }

    private String synchronizeMode() {
        LocalDateTime arduinoTime = arduinoModeLastModified;
        LocalDateTime serviceTime = dataService.getModeLastModifiedTime();

        if (!Objects.equals(arduino_mode, dataService.getCurrentMode())) {
            if (arduinoTime == null || serviceTime == null) {
                return arduino_mode;
            }
    
            return arduinoTime.isAfter(serviceTime)
                    ? arduino_mode
                    : dataService.getCurrentMode();
        }
        return arduino_mode;
    }

    private void sendMode(String mode) {
        this.serialChannel.sendMsg("MODE:" + mode);
    }

    private void sendTemperature(double temp) {
        this.serialChannel.sendMsg("TEMP:" + temp);
    }

    private void sendPosition(int pos) {
        this.serialChannel.sendMsg("POS:" + pos);
    }
}
