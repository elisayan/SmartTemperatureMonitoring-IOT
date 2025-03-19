package control.unit.backed;

import io.vertx.core.Vertx;

public class App {

    public static void main(String[] args) throws Exception {
        Vertx vertx = Vertx.vertx();
        DataService dataService = new DataService(8080);
        MQTTAgent mqttAgent = new MQTTAgent();

        vertx.deployVerticle(dataService);
        vertx.deployVerticle(mqttAgent);
    }
}
