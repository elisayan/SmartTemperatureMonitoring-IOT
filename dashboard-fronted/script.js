const ctx = document.getElementById('tempChart').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Temperature (°C)',
            data: [],
            borderColor: 'rgb(75, 192, 192)',
            fill: false
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
                }
            }
        }
    }
});

async function update() {
    try {
        const res = await fetch('/api/data');
        const data = await res.json();
        
        chart.data.labels = data.map(d => new Date(d.time).toLocaleTimeString());
        chart.data.datasets[0].data = data.map(d => d.value);
        chart.update();
        
        const stateRes = await fetch('/api/state');
        const state = await stateRes.json();
        document.getElementById('mode').textContent = state.mode;
        document.getElementById('temp').textContent = state.window.toFixed(2);
    } catch (error) {
        console.error('Error updating data:', error);
    }
}

async function setManual() {
    const pos = document.getElementById('slider').value;
    try {
        await fetch('/api/mode', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode: "MANUAL", position: parseInt(pos) })
        });
    } catch (error) {
        console.error('Error setting manual mode:', error);
    }
}

async function resolveAlarm() {
    try {
        await fetch('/api/alarm', { method: 'POST' });
    } catch (error) {
        console.error('Error resolving alarm:', error);
    }
}

document.getElementById('alarm').textContent = state.alarm;

setInterval(update, 2000);

update();