package control.unit.backed;

import io.vertx.ext.web.handler.CorsHandler;
import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpHeaders;
import io.vertx.core.http.HttpMethod;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import java.util.concurrent.CopyOnWriteArrayList;

public class DataService extends AbstractVerticle {
    private int port;
    private static final int MAX_SIZE = 50;
    private CopyOnWriteArrayList<DataPoint> temperatureData;
    private SystemState systemState;
    private SerialCommChannel serialChannel;
    private String lastManualCommandSource = null;

    public DataService(int port, SerialCommChannel serialChannel) throws Exception {
        temperatureData = new CopyOnWriteArrayList<>();
        systemState = new SystemState();
        this.port = port;
        this.serialChannel = serialChannel;
    }

    private static class SystemState {
        volatile String mode = "AUTOMATIC";
        volatile int windowPosition = 0;
        volatile String state = "NORMAL";
    }

    @Override
    public void start() throws InterruptedException {
        Router router = Router.router(vertx);

        router.route().handler(CorsHandler.create()
                .addOrigin("*")
                .allowedMethod(HttpMethod.GET)
                .allowedMethod(HttpMethod.POST)
                .allowedMethod(HttpMethod.OPTIONS)
                .allowedHeader(HttpHeaders.CONTENT_TYPE.toString()));

        router.route().handler(BodyHandler.create());

        router.post("/api/data").handler(this::handleAddNewData);
        router.get("/api/data").handler(this::handleGetTemperatureData);

        router.get("/api/state").handler(this::handleGetCurrentState);

        router.post("/api/mode").handler(this::handleModeChange);

        vertx.createHttpServer().requestHandler(router).listen(port);

        vertx.createHttpServer().requestHandler(router).listen(port);

        vertx.setPeriodic(20, timerId -> {
            while (serialChannel.isMsgAvailable()) {
                try {
                    String msg = serialChannel.receiveMsg();
                    if (msg != null) {
                        handleSerialMessage(msg);
                    }
                } catch (InterruptedException e) {
                    System.err.println("Thread interrotto durante la ricezione del messaggio: " + e.getMessage());
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        });
    }

    private void handleSerialMessage(String msg) {
        if (msg.startsWith("TEMP:")) {
            double temp = Double.parseDouble(msg.substring(5));
            addTemperatureData(temp);
        } else if (msg.startsWith("POS:")) {
            int pos = Integer.parseInt(msg.substring(4));
            systemState.windowPosition = pos;
        }
    }

    private void handleAddNewData(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body != null) {
            double value = body.getDouble("value");
            String place = body.getString("place", "unknown");
            long time = System.currentTimeMillis();

            temperatureData.addLast(new DataPoint(value, time, place));
            if (temperatureData.size() > MAX_SIZE) {
                temperatureData.removeFirst();
            }

            ctx.response().setStatusCode(200).end();
        } else {
            ctx.response().setStatusCode(400).end("Invalid data");
        }
    }

    private void handleGetTemperatureData(RoutingContext ctx) {
        JsonArray arr = new JsonArray();
        temperatureData.forEach(p -> {
            arr.add(new JsonObject()
                    .put("time", p.getTime())
                    .put("value", p.getValue()));
        });
        ctx.response()
                .putHeader("content-type", "application/json")
                .end(arr.encodePrettily());
    }

    public void addTemperatureData(double value) {
        temperatureData.addLast(new DataPoint(value, System.currentTimeMillis(), "sensor"));
        if (temperatureData.size() > MAX_SIZE) {
            temperatureData.removeFirst();
        }
    }

    private void handleGetCurrentState(RoutingContext ctx) {
        JsonObject state = new JsonObject()
                .put("mode", systemState.mode)
                .put("window", systemState.windowPosition)
                .put("state", systemState.state)
                .put("lastManualCommandSource", lastManualCommandSource);
        ctx.response().end(state.encodePrettily());
    }

    @SuppressWarnings("deprecation")
    private void handleModeChange(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body != null) {
            String mode = body.getString("mode");
            int position = body.getInteger("position");
            String source = body.getString("source", "Dashboard");

            vertx.executeBlocking(promise -> {
                serialChannel.sendMsg("MODE:" + mode);
                serialChannel.sendMsg("POS:" + position);
                promise.complete();
            }, false, res -> {
                if (res.succeeded()) {
                    systemState.mode = mode;
                    systemState.windowPosition = position;
                    if (mode.equals("MANUAL")) {
                        lastManualCommandSource = source;
                    } else {
                        lastManualCommandSource = null;
                    }
                    ctx.response().end("OK");
                } else {
                    ctx.response().setStatusCode(500).end("Errore interno");
                }
            });
        } else {
            ctx.response().setStatusCode(400).end("Invalid request body");
        }
    }

    public void updateState(int windowPos, String state) {
        systemState.windowPosition = windowPos;
        systemState.state = state;
    }
}
