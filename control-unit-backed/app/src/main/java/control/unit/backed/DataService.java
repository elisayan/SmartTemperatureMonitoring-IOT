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
    private String lastManualCommandSource = null;
    private Controller controller;
    private String dashboardMode = "AUTOMATIC";
    private int dashboardPosition = 0;
    private String dashboardState = "NORMAL";

    public DataService(int port) throws Exception {
        temperatureData = new CopyOnWriteArrayList<>();
        this.port = port;
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

    public void setController(Controller controller) {
        this.controller = controller;
    }

    public void addTemperatureData(double temp) {
        temperatureData.addLast(new DataPoint(temp, System.currentTimeMillis(), "sensor"));
        if (temperatureData.size() > MAX_SIZE) {
            temperatureData.removeFirst();
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

    // private void handleGetTemperatureData(RoutingContext ctx) {
    // JsonArray arr = new JsonArray();
    // temperatureData.forEach(p -> {
    // arr.add(new JsonObject()
    // .put("time", p.getTime())
    // .put("value", p.getValue()));
    // });
    // ctx.response()
    // .putHeader("content-type", "application/json")
    // .end(arr.encodePrettily());
    // }

    private void handleGetTemperatureData(RoutingContext ctx) {
        int n = 5;
        double averageTemperature = calculateAverage(n);

        // JsonObject response = new JsonObject()
        // .put("averageTemperature", averageTemperature)
        // .put("numberOfDataPoints", n);

        JsonArray arr = new JsonArray();

        temperatureData.forEach(p -> {
            arr.add(new JsonObject().put("time", p.getTime())
                    .put("averageTemperature", averageTemperature)
                    .put("numberOfDataPoints", n));
        });

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(arr.encodePrettily());
    }

    private void handleGetCurrentState(RoutingContext ctx) {
        JsonObject state = new JsonObject()
                .put("mode", dashboardMode)
                .put("window", dashboardPosition)
                .put("state", dashboardState)
                .put("lastManualCommandSource", lastManualCommandSource);
        ctx.response().end(state.encodePrettily());
    }

    private void handleModeChange(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body == null) {
            ctx.response().setStatusCode(400).end("Invalid request body");
            return;
        }

        dashboardMode = body.getString("mode");
        dashboardPosition = dashboardMode.equals("MANUAL") ? body.getInteger("position") : dashboardPosition;
        dashboardState = body.getString("source", "Dashboard");

        controller.synchronizeArduinoWithDataService();

        ctx.response().end("OK");
    }

    private double calculateAverage(int n) {
        if (temperatureData.isEmpty() || n <= 0) {
            return 0.0;
        }

        int size = Math.min(n, temperatureData.size());
        double sum = 0.0;

        for (int i = temperatureData.size() - size; i < temperatureData.size(); i++) {
            sum += temperatureData.get(i).getValue();
        }

        return sum / size;
    }

    public void updateWindow(int windowPos) {
        dashboardPosition = windowPos;
    }

    public void updateState(String state) {
        dashboardState = state;
    }

    public void updateMode(String mode) {
        dashboardMode = mode;
    }

    public String getCurrentMode() {
        return dashboardMode;
    }

    public int getDashboardPosition() {
        return dashboardPosition;
    }
}
