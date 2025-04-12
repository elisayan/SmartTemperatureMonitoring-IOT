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

public class DataService extends AbstractVerticle {
    private static final int MAX_SIZE = 50;
    private static final int N = 5;

    private Controller controller;
    private int port;
    private List<DataPoint> averageData;
    private List<DataPoint> temperatureData = new LinkedList<>();

    private String dashboardMode = "AUTOMATIC";
    private String dashboardState = "NORMAL";
    private int dashboardPosition = 0;

    private LocalDateTime dashboardModeLastModified;
    private LocalDateTime dashboardPosLastModified;

    public DataService(int port) throws Exception {
        averageData = new LinkedList<>();
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

        vertx.createHttpServer().requestHandler(router).listen(port);
    }

    public void addTemperatureData(double temp) {
        temperatureData.add(new DataPoint(temp, System.currentTimeMillis(), "sensor"));
        updateAverageData();
    }

    private void handleGetTemperatureData(RoutingContext ctx) {
        if (temperatureData.size() == N) {
            double lastAverage = averageData.getLast().getValue();

            JsonObject response = new JsonObject()
                    .put("averageTemperature", lastAverage)
                    .put("numberOfDataPoints", temperatureData.size());

            ctx.response()
                    .putHeader("content-type", "application/json")
                    .end(response.encodePrettily());

            temperatureData.clear();
        } else {
            ctx.response()
            .setStatusCode(404)
            .setStatusMessage("No data available")
            .end();
        }
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

        dashboardMode = body.getString("mode");
        dashboardModeLastModified = LocalDateTime.now();
        dashboardPosition = dashboardMode.equals("MANUAL") ? body.getInteger("position") : dashboardPosition;

        controller.synchronizeAndUpdateMode();

        ctx.response().setStatusMessage("OK").end();
    }

    private double calculateAverage() {
        if (temperatureData.isEmpty()) {
            return 0.0;
        }

        double sum = 0.0;
        for (DataPoint data : temperatureData) {
            sum += data.getValue();
        }

        return sum / temperatureData.size();
    }

    private void updateAverageData() {
        if (temperatureData.size() == N) {
            double average = calculateAverage();

            averageData.add(new DataPoint(average, System.currentTimeMillis(), "average"));

            if (averageData.size() > MAX_SIZE) {
                averageData.removeFirst();
            }
        }
    }

    public void updateWindow(int windowPos) {
        dashboardPosition = windowPos;
        if (dashboardMode.equals("MANUAL")) {
            dashboardPosLastModified = LocalDateTime.now();
        }
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
