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
        await fetch('http://localhost:8080/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "MANUAL", position: parseInt(position), source: "Dashboard" })
        });
    } catch (error) {
        console.error('Error updating window position:', error);
    }
}

// async function update() {
//     try {
//         const res = await fetch('http://localhost:8080/api/data');
//         const data = await res.json();
        
//         chart.data.labels = data.map(d => new Date(d.time).toLocaleTimeString());
//         chart.data.datasets[0].data = data.map(d => d.value);
//         chart.update();
        
//         const stateRes = await fetch('http://localhost:8080/api/state');
//         const state = await stateRes.json();
//         document.getElementById('mode').textContent = state.mode;
//         document.getElementById('window').textContent = state.window.toFixed(2);
//         document.getElementById('state').textContent = state.state;
//         updateButtonText(state.mode);
//     } catch (error) {
//         console.error('Error updating data:', error);
//     }
// }

async function update() {
    try {
        const res = await fetch('http://localhost:8080/api/data');
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

        const stateRes = await fetch('http://localhost:8080/api/state');
        const state = await stateRes.json();
        document.getElementById('mode').textContent = state.mode;
        document.getElementById('window').textContent = state.window.toFixed(2);
        document.getElementById('state').textContent = state.state;
        updateButtonText(state.mode);
    } catch (error) {
        console.error('Error updating data:', error);
    }
}

async function setManual() {
    const pos = slider.value;
    try {
        await fetch('http://localhost:8080/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "MANUAL", position: parseInt(pos), source: "Dashboard" })
        });
    } catch (error) {
        console.error('Error setting manual mode:', error);
    }
}

async function setAutomatic() {
    try {
        await fetch('http://localhost:8080/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "AUTOMATIC", position: 0, source: "Dashboard" })
        });
    } catch (error) {
        console.error('Error setting automatic mode:', error);
    }
}

async function resolveAlarm() {
    try {
        await fetch('http://localhost:8080/api/alarm', { method: 'POST' });
    } catch (error) {
        console.error('Error resolving alarm:', error);
    }
}

setInterval(update, 2000);
update();
