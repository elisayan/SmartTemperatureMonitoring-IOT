package control.unit.backed;

public class Controller {

    private static final String PORT = "COM3";
    private static final int RATE = 115200;

    private final MQTTAgent mqtt;
    private final DataService dataService;
    private final SerialCommChannel serialChannel;

    private String arduino_mode = null;
    private String mode = null;
    // private double temperature = -1;
    private int arduino_pos = -1;
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
        } else if (msg.startsWith("POS:")) {
            arduino_pos = Integer.parseInt(msg.split(":")[1].trim());
        }
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

    private void sendSource(String source) {
        this.serialChannel.sendMsg("SOURCE:" + source);
    }

    private void synchronizeDataServiceWithArduino() {
        this.dataService.updateMode(mode);
        this.dataService.updateWindow(pos);
        System.out.println("DASHBOARD SYCRON: " + "MODE: " + mode + " POS: " + pos);
    }

    public void synchronizeArduinoWithDataService() {
        // sendSource("DATASERVICE");
        // sendMode(this.dataService.getCurrentMode());
        // sendPosition(this.dataService.getDashboardPosition());
    }

    public void updateSystem(double temp, int position, String state) {
        // this.dataService.addTemperatureData(temp);
        // this.dataService.updateWindow(position);
        // this.dataService.updateState(state);

        if (arduino_mode.equals("MANUAL")) {
            sendTemperature(temp);
            pos = arduino_pos;
            System.out.println("UPDATE MANUAL TEMP");
        }
        sendPosition(position);

        System.out.println(
                "UPDATE: " + "MODE: " + arduino_mode + " TEMP: " + temp + " POS: " + position + " STATE: " + state);
    }
}
