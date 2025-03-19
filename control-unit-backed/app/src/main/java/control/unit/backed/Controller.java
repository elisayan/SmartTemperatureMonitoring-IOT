package control.unit.backed;

public class Controller {

    private static final String PORT = "COM3";
    private static final int RATE = 115200;

    private final MQTTAgent mqtt;
    private final DataService dataService;
    private final SerialCommChannel serialChannel;

    private String mode = null;
    private double temperature = -1;
    private int windowPosition = -1;

    public Controller(MQTTAgent mqtt, DataService dataService) throws Exception {
        this.mqtt = mqtt;
        this.dataService = dataService;
        serialChannel = new SerialCommChannel(PORT, RATE, this);

        this.mqtt.setController(this);
        this.dataService.setController(this);
    }

    public void receiveMsg(String msg) {
        if (msg.startsWith("MODE:")) {
            mode = msg.split(":")[1].trim();
            //dataService.handleModeChange(mode);
        } else if (msg.startsWith("TEMP:")) {
            temperature = Double.parseDouble(msg.split(":")[1].trim());
        } else if (msg.startsWith("POS:")) {
            windowPosition = Integer.parseInt(msg.split(":")[1].trim());
        } else if (msg.startsWith("SOURCE:")) {
            String source = msg.split(":")[1].trim();
            if (source.equals("ARDUINO")) {
                synchronizeDataServiceWithArduino();
            } else if (source.equals("DATASERVICE")) {
                synchronizeArduinoWithDataService();
            }
        }
    }

    public void sendMode(String mode) {
        this.serialChannel.sendMsg("MODE:" + mode);
    }

    public void sendTemperature(double temp) {
        this.serialChannel.sendMsg("TEMP:" + temp);
    }

    public void sendPosition(int pos) {
        this.serialChannel.sendMsg("POS:" + pos);
    }

    public void sendSource(String source) {
        this.serialChannel.sendMsg("SOURCE:" + source);
    }

    public String getMode(){
        return this.mode;
    }

    private void synchronizeDataServiceWithArduino() {
        sendSource("DATASERVICE");
        sendMode(dataService.getCurrentMode());
        sendPosition(dataService.getDashboardPosition());
    }

    private void synchronizeArduinoWithDataService(){
        
    }

    public void updateSystem(double temp, int position, String state) {
        dataService.addTemperatureData(temp);
        dataService.updateState(position, state);

        sendTemperature(temp);
        sendPosition(position);
    }
}
