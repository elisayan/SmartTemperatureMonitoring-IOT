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
        String mode = "AUTOMATIC";
        int windowPosition = 0;
        String state = "NORMAL";
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

    private void handleModeChange(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body == null) {
            ctx.response().setStatusCode(400).end("Invalid request body");
            return;
        }

        String mode = body.getString("mode");
        int position = mode.equals("MANUAL") ? body.getInteger("position") : systemState.windowPosition;
        String source = body.getString("source", "Dashboard");

        new Thread(() -> {
            try {
                serialChannel.sendMsg("MODE:" + mode);
                if (mode.equals("MANUAL")) {
                    serialChannel.sendMsg("POS:" + position);
                }
                serialChannel.sendMsg("POS:" + position);
                System.out.println("http send to arduino:\nMODE: " + mode + "\nPOSITION: " + position);

                // Attendi una risposta dalla seriale (opzionale)
                if (serialChannel.isMsgAvailable()) {
                    String msg = serialChannel.receiveMsg();
                    // Puoi gestire la risposta qui se necessario
                }

                systemState.mode = mode;
                systemState.windowPosition = position;

                if (mode.equals("MANUAL")) {
                    lastManualCommandSource = source;
                } else {
                    lastManualCommandSource = null;
                }

                ctx.response().end("OK");
            } catch (Exception e) {
                e.printStackTrace();
                ctx.response().setStatusCode(500).end("Errore interno");
            }

        }).start();
    }

    public void updateState(int windowPos, String state) {
        systemState.windowPosition = windowPos;
        systemState.state = state;
    }

    public String getCurrentMode() {
        return systemState.mode;
    }
}
