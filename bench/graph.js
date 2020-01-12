const chartjs = require('chartjs-node');
const fs = require('fs');
const path = require('path');
const tests = require('./tests');

const colours = {
  'hyperbuild': '#041f60',
  'hyperbuild-nodejs': '#1f77b4',
  'minimize': '#ff7f0e',
  'html-minifier': '#2ca02c',
};

const programNames = ['minimize', 'html-minifier', 'hyperbuild-nodejs'];

const chartOptions = (title, displayLegend, yTick = t => t) => ({
  options: {
    title: {
      display: true,
      text: title,
      fontColor: '#333',
    },
    scales: {
      xAxes: [{
        barPercentage: 0.5,
        gridLines: {
          color: '#e2e2e2',
        },
        ticks: {
          fontColor: '#666',
        },
      }],
      yAxes: [{
        gridLines: {
          color: '#ccc',
        },
        ticks: {
          callback: yTick,
          fontColor: '#666',
        },
      }],
    },
    legend: {
      display: displayLegend,
      labels: {
        fontFamily: 'Arial, sans-serif',
        fontColor: '#000',
      },
    },
  },
});

const renderChart = async (file, cfg) => {
  const chart = new chartjs(435, 320);
  await chart.drawChart(cfg);
  await chart.writeImageToFile('image/png', path.join(__dirname, `${file}.png`));
};

(async () => {
  const testNames = tests.map(t => t.name).sort();

  const speedResults = JSON.parse(fs.readFileSync(path.join(__dirname, 'speed.json'), 'utf8'));
  const speedData = programNames.map(program => [
    program,
    testNames
      // Get OP/s for each test.
      .map(test => speedResults[test][program] / speedResults[test]['hyperbuild-nodejs'])
      // Sum all test OP/s.
      .reduce((sum, c) => sum + c)
    // Divide by tests count to get average OP/s.
    / testNames.length,
  ]).sort((a, b) => a[1] - b[1]);
  await renderChart('speed', {
    type: 'bar',
    data: {
      labels: speedData.map(([n]) => n),
      // Node.js version is close enough to Rust version, so leave out Rust results.
      // Include it this if this situation changes.
      datasets: [{
        label: 'Average relative OP/s',
        backgroundColor: '#0366d6',
        data: speedData.map(([_, v]) => v),
      }],
    },
    ...chartOptions('Average operations per second (higher is better)', false, tick => `${tick * 100}%`),
  });

  const sizes = JSON.parse(fs.readFileSync(path.join(__dirname, 'minification.json'), 'utf8'));
  const sizeData = programNames.map(program => [
    program,
    testNames
      .map(test => sizes[test][program].relative)
      .reduce((sum, c) => sum + c)
    / testNames.length,
  ]).sort((a, b) => b[1] - a[1]);
  await renderChart('minification', {
    type: 'bar',
    scaleFontColor: 'red',
    data: {
      labels: sizeData.map(([n]) => n),
      datasets: [{
        label: 'Average minified size',
        backgroundColor: '#0366d6',
        data: sizeData.map(([_, v]) => v),
      }],
    },
    ...chartOptions('Average minified size (lower is better)', false, tick => `${tick * 100}%`),
  });
})();
