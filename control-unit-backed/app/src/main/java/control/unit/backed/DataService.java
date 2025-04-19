package control.unit.backed;

import io.vertx.ext.web.handler.CorsHandler;
import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpHeaders;
import io.vertx.core.http.HttpMethod;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;

import java.time.LocalDateTime;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

public class DataService extends AbstractVerticle {
    private static final int MAX_SIZE = 50;
    private static final int N = 5;

    private Controller controller;
    private int port;
    private List<DataPoint> temperatureData = new LinkedList<>();

    private double maxTemperature = Double.MIN_VALUE;
    private double minTemperature = Double.MAX_VALUE;

    private String dashboardMode = "AUTOMATIC";
    private String dashboardState = "NORMAL";
    private int dashboardPosition = 0;

    private LocalDateTime dashboardModeLastModified = null;
    private LocalDateTime dashboardPosLastModified = null;

    public DataService(int port) throws Exception {
        this.port = port;
    }

    public void setController(Controller controller) {
        this.controller = controller;
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

        router.get("/api/data").handler(this::handleGetTemperatureData);

        router.get("/api/state").handler(this::handleGetCurrentState);

        router.post("/api/mode").handler(this::handleModeChange);

        router.post("/api/alarm").handler(this::handleAlarmResolution);

        vertx.createHttpServer().requestHandler(router).listen(port);
    }

    public void addTemperatureData(double temp) {
        temperatureData.add(new DataPoint(temp, System.currentTimeMillis(), "sensor"));

        if (temp > maxTemperature) maxTemperature = temp;
        if (temp < minTemperature) minTemperature = temp;
        
        if (temperatureData.size() > MAX_SIZE) {
            temperatureData.remove(0);
        }
    }

    private void handleGetTemperatureData(RoutingContext ctx) {
        if (temperatureData.isEmpty()) {
            ctx.response()
                    .setStatusCode(404)
                    .setStatusMessage("No data available")
                    .end();
            return;
        }
    
        JsonObject response = new JsonObject()
                .put("averageTemperature", calculateAverage())
                .put("currentTemperature", getCurrentTemperature())
                .put("maxTemperature", maxTemperature)
                .put("minTemperature", minTemperature)
                .put("temperatures", getLastNTemperatures(N))
                .put("numberOfDataPoints", temperatureData.size());
    
        ctx.response()
                .putHeader("content-type", "application/json")
                .end(response.encodePrettily());
    }

    private double getCurrentTemperature() {
        return temperatureData.getLast().getValue();
    }

    private void handleGetCurrentState(RoutingContext ctx) {
        JsonObject state = new JsonObject()
                .put("mode", dashboardMode)
                .put("window", dashboardPosition)
                .put("state", dashboardState);
        ctx.response().end(state.encodePrettily());
    }

    private void handleModeChange(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body == null) {
            ctx.response().setStatusCode(400).setStatusMessage("Invalid request body").end();
            return;
        }
    
        String newMode = body.getString("mode");
    
        if (!newMode.equals(dashboardMode)) {
            dashboardMode = newMode;
            dashboardModeLastModified = LocalDateTime.now();    
            controller.synchronizeAndUpdateMode();
        }
        dashboardPosition = dashboardMode.equals("MANUAL") ? body.getInteger("position") : dashboardPosition;
    
        ctx.response().setStatusMessage("OK").end();
    }

    private void handleAlarmResolution(RoutingContext ctx) {
        controller.resolveAlarm();
        JsonObject response = new JsonObject()
            .put("status", "success")
            .put("message", "Alarm resolved successfully");
        ctx.response()
            .putHeader("content-type", "application/json")
            .end(response.encodePrettily());
    }

    private List<Double> getLastNTemperatures(int n) {
        return temperatureData.stream()
                .skip(Math.max(0, temperatureData.size() - n))
                .map(DataPoint::getValue)
                .collect(Collectors.toList());
    }

    private double calculateAverage() {
        if (temperatureData.isEmpty()) return 0.0;
        
        return getLastNTemperatures(N).stream()
                .mapToDouble(Double::doubleValue)
                .average()
                .orElse(0.0);
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

    public LocalDateTime getModeLastModifiedTime() {
        return dashboardModeLastModified;
    }

    public LocalDateTime getDashboardPosLastModifiedTime() {
        return dashboardPosLastModified;
    }
}
