const ctx = document.getElementById('tempChart').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Temperature (°C)',
            data: [],
            borderColor: 'rgb(75, 192, 192)',
            fill: false,
            pointRadius: 5,
            pointBackgroundColor: 'rgb(75, 192, 192)'
        }]
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

        if(!res.ok){
            throw new Error(`Response status: ${res.status}`);
        }
    } catch (error) {
        console.error(error.message);
    }
}

async function updateData() {
    try {
        const res = await fetch('http://localhost:8080/api/data');
        if(!res.ok){
            throw new Error(`Response status: ${res.status}`);
        }

        const data = await res.json();

        const currentTime = new Date().toLocaleTimeString();
        chart.data.labels.push(currentTime);
        chart.data.datasets[0].data.push(data.averageTemperature);

        const maxDataPoints = 10;
        if (chart.data.labels.length > maxDataPoints) {
            chart.data.labels.shift();
            chart.data.datasets[0].data.shift();
        }

        chart.update();
    } catch (error) {
        console.error(error.message);
    }
}

async function updateState() {
    try {
        const stateRes = await fetch('http://localhost:8080/api/state');
        if(!stateRes.ok){
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

        if(!res.ok){
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
        if(!res.ok){
            throw new Error(`Response status: ${res.status}`);
        }
    } catch (error) {
        console.error(error.message);
    }
}

async function resolveAlarm() {
    try {
        const res = await fetch('http://localhost:8080/api/alarm', { method: 'POST' });
        if(!res.ok){
            throw new Error(`Response status: ${res.status}`);
        }
    } catch (error) {
        console.error(error.message);
    }
}

setInterval(updateData, 1000);
setInterval(updateState, 1000);
updateData();
updateState();
