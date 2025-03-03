package control.unit.backed;

import io.vertx.core.Vertx;

public class App {

    public static void main(String[] args) throws Exception {
        Vertx vertx = Vertx.vertx();
        SerialCommChannel serialChannel = new SerialCommChannel("COM3", 115200);
        DataService dataService = new DataService(8080, serialChannel);
        MQTTAgent mqttAgent = new MQTTAgent(dataService, serialChannel);
        vertx.deployVerticle(dataService);
        vertx.deployVerticle(mqttAgent);
    }
}
