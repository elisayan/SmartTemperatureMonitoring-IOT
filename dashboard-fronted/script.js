const ctx = document.getElementById('tempChart').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [
            {
                label: 'Current Temperature (°C)',
                data: [],
                borderColor: 'rgb(75, 192, 192)',
                fill: false,
                pointRadius: 5,
                pointBackgroundColor: 'rgb(75, 192, 192)'
            }
        ]
    },
    options: {
        responsive: true,
        scales: {
            x: {
                display: true,
                title: {
                    display: true,
                    text: 'Time'
                }
            },
            y: {
                display: true,
                title: {
                    display: true,
                    text: 'Temperature (°C)'
                },
                suggestedMin: 10,
                suggestedMax: 30
            }
        }
    }
});

const modeButton = document.getElementById('modeButton');
const slider = document.getElementById('slider');

function updateButtonText(mode) {
    if (mode === "MANUAL") {
        modeButton.textContent = "Switch to Automatic Mode";
        modeButton.onclick = setAutomatic;
    } else {
        modeButton.textContent = "Switch to Manual Mode";
        modeButton.onclick = setManual;
    }
}

modeButton.addEventListener('click', function () {
    if (modeButton.onclick) {
        modeButton.onclick();
    }
});

slider.addEventListener('input', function () {
    const currentMode = document.getElementById('mode').textContent;
    if (currentMode === "MANUAL") {
        updateWindowPosition(slider.value);
    }
});

async function updateWindowPosition(position) {
    try {
        const res = await fetch('http://localhost:8080/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "MANUAL", position: parseInt(position), source: "Dashboard" })
        });

        if (!res.ok) {
            throw new Error(`Response status: ${res.status}`);
        }
    } catch (error) {
        console.error(error.message);
    }
}

async function updateData() {
    try {
        const res = await fetch('http://localhost:8080/api/data');
        if (!res.ok) throw new Error(`Response status: ${res.status}`);

        const data = await res.json();

        chart.data.labels = Array.from({length: data.temperatures.length}, (_, i) => `M${i+1}`);
        chart.data.datasets[0].data = data.temperatures;
        
        chart.update();
        
        document.getElementById('avgTemp').textContent = data.averageTemperature.toFixed(2) + '°C';
        document.getElementById('maxTemp').textContent = data.maxTemperature.toFixed(2) + '°C';
        document.getElementById('minTemp').textContent = data.minTemperature.toFixed(2) + '°C';
        
    } catch (error) {
        console.error(error.message);
    }
}

async function updateState() {
    try {
        const stateRes = await fetch('http://localhost:8080/api/state');
        if (!stateRes.ok) {
            throw new Error(`Response status: ${stateRes.status}`);
        }

        const state = await stateRes.json();

        document.getElementById('mode').textContent = state.mode;
        document.getElementById('window').textContent = state.window.toFixed(2);
        document.getElementById('state').textContent = state.state;

        updateButtonText(state.mode);

        const alarmButton = document.querySelector('#status button');
        if (state.state === "ALARM") {
            alarmButton.disabled = false;
        } else {
            alarmButton.disabled = true;
        }
    } catch (error) {
        console.error(error.message);
    }
}

async function setManual() {
    const pos = slider.value;
    try {
        const res = await fetch('http://localhost:8080/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "MANUAL", position: parseInt(pos), source: "Dashboard" })
        });

        if (!res.ok) {
            throw new Error(`Response status: ${res.status}`);
        }
    } catch (error) {
        console.error(error.message);
    }
}

async function setAutomatic() {
    try {
        const res = await fetch('http://localhost:8080/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "AUTOMATIC", position: 0, source: "Dashboard" })
        });
        if (!res.ok) {
            throw new Error(`Response status: ${res.status}`);
        }
    } catch (error) {
        console.error(error.message);
    }
}

async function resolveAlarm() {
    try {
        const res = await fetch('http://localhost:8080/api/alarm', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' }
        });

        const result = await res.json();
        if (result.status === 'success') {
            showTempAlert(result.message, 'success');
        }
    } catch (error) {
        showTempAlert('Failed to resolve alarm', 'danger');
        console.error(error.message);
    }
}

function showTempAlert(message, type) {
    const alertDiv = document.createElement('div');
    alertDiv.className = `alert alert-${type} alert-dismissible fade show`;
    alertDiv.role = 'alert';
    alertDiv.innerHTML = `
        ${message}
        <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    `;

    const container = document.querySelector('.container-fluid');
    container.prepend(alertDiv);

    setTimeout(() => {
        alertDiv.remove();
    }, 3000);
}

setInterval(updateData, 2000);
updateData();

setInterval(updateState, 1000);
updateState();
