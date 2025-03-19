package control.unit.backed;

public class Controller {

    private static final String PORT = "COM3";
    private static final int RATE = 115200;

    private final MQTTAgent mqtt;
    private final DataService dataService;
    private final SerialCommChannel serialChannel;
    
    public Controller(MQTTAgent mqtt, DataService dataService) throws Exception{
        this.mqtt = mqtt;
        this.dataService = dataService;
        serialChannel = new SerialCommChannel(PORT, RATE, this);

        this.mqtt.setController(this);
        this.dataService.setController(this);
    }

    public void receiveMsg(String msg) {
        if (msg.startsWith("MODE:")) {
            
        } else if (msg.startsWith("TEMP:")){

        } else if (msg.startsWith("POS:")){

        }
    }


}
