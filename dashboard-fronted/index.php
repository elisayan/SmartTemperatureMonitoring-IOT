<!DOCTYPE html>
<html lang="it">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Temperature Dashboard</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link rel="stylesheet" href="style.css">
</head>
<body class="bg-light">
    <header class="text-center py-4">
        <div class="container">
            <h1 class="display-4">Smart Temperature Monitoring</h1>
            <p class="lead">Monitor and control your environment in real-time</p>
        </div>
    </header>

    <div class="container-fluid py-4">
        <div class="row">
            <div class="col-md-8">
                <div class="card shadow-sm">
                    <div class="card-body">
                        <canvas id="tempChart"></canvas>
                    </div>
                </div>
            </div>
            <div class="col-md-4">
                <div class="card shadow-sm">
                    <div class="card-body">
                        <h2 class="card-title">Controls</h2>
                        <div class="mb-3">
                            <label for="slider" class="form-label">Window Position:</label>
                            <input type="range" class="form-range" id="slider" min="0" max="100">
                        </div>
                        <button class="btn btn-primary w-100 mb-3" onclick="setManual()">Manual Mode</button>
                        <div id="status">
                            <p class="mb-1">Current Mode: <span id="mode" class="fw-bold">AUTO</span></p>
                            <p class="mb-0">Window: <span id="window" class="fw-bold">-</span>%</p>
                            <p class="mb-1">State: <span id="state" class="fw-bold">NORMAL</span></p>
                            <button class="btn btn-danger mt-2" onclick="resolveAlarm()">Resolve Alarm</button>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
    <script src="script.js"></script>
</body>
</html>