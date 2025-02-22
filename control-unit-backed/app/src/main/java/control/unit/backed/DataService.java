package control.unit.backed; 

import io.vertx.core.AbstractVerticle;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import java.util.LinkedList;

public class DataService extends AbstractVerticle{

    private int port;
    private static final int MAX_SIZE = 50;
    private LinkedList<DataPoint> temperatureData;
    private SystemState systemState;

    public DataService(int port) {
        temperatureData = new LinkedList<>();
        systemState = new SystemState();
        this.port = port;
    }

    private static class SystemState {
        String mode = "AUTOMATIC";
        int windowPosition = 0;
        String alarm = "NORMAL";
    }

    @Override
    public void start() {
        Router router = Router.router(vertx);
        router.route().handler(BodyHandler.create());
        
        router.get("/api/temperature").handler(this::handleGetTemperatureData);
        
        router.get("/api/state").handler(this::handleGetCurrentState);
        
        router.post("/api/mode").handler(this::handleModeChange);

        vertx.createHttpServer().requestHandler(router).listen(port);
    }

    public void addTemperatureData(double value) {
        temperatureData.addFirst(new DataPoint(value, System.currentTimeMillis(), "sensor"));
        if (temperatureData.size() > MAX_SIZE) {
            temperatureData.removeLast();
        }
    }

    private void handleGetTemperatureData(RoutingContext ctx) {
        JsonArray arr = new JsonArray();
        for (DataPoint p : temperatureData) {
            arr.add(new JsonObject()
                .put("time", p.getTime())
                .put("value", p.getValue()));
        }
        ctx.response()
            .putHeader("content-type", "application/json")
            .end(arr.encodePrettily());
    }

    private void handleGetCurrentState(RoutingContext ctx) {
        JsonObject state = new JsonObject()
            .put("mode", systemState.mode)
            .put("window", systemState.windowPosition)
            .put("alarm", systemState.alarm);
        
        ctx.response().end(state.encodePrettily());
    }

    private void handleModeChange(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body != null) {
            systemState.mode = body.getString("mode");
            systemState.windowPosition = body.getInteger("position");
            ctx.response().end("OK");
        } else {
            ctx.response()
                .setStatusCode(400)
                .end("Invalid request body");
        }
    }
}
