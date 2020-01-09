const chartjs = require('chartjs-node');
const fs = require('fs');
const path = require('path');
const tests = require('./tests');

const colours = {
  'hyperbuild': '#041f60',
  'hyperbuild-nodejs': '#0476d0',
  'minimize': '#3cacae',
  'html-minifier': '#5d6c89',
};

const chartOptions = (title, displayLegend, yTick = t => t) => ({
  options: {
    title: {
      display: true,
      text: title,
    },
    scales: {
      xAxes: [{
        barPercentage: 0.75,
        gridLines: {
          color: '#ccc',
        },
        ticks: {
          fontColor: '#222',
        },
      }],
      yAxes: [{
        gridLines: {
          color: '#666',
        },
        ticks: {
          callback: yTick,
          fontColor: '#222',
        },
      }],
    },
    legend: {
      display: displayLegend,
      labels: {
        fontFamily: 'Ubuntu, sans-serif',
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
  await renderChart('speed', {
    type: 'bar',
    data: {
      labels: testNames,
      datasets: ['hyperbuild', 'hyperbuild-nodejs', 'minimize', 'html-minifier'].map(program => ({
        label: program,
        backgroundColor: colours[program],
        data: testNames.map(test => speedResults[test][program] / speedResults[test]['hyperbuild'] * 100),
      })),
    },
    ...chartOptions('Relative operations per second (higher is better)', true, tick => `${tick}%`),
  });

  const sizes = JSON.parse(fs.readFileSync(path.join(__dirname, 'minification.json'), 'utf8'));
  await renderChart('minification', {
    type: 'bar',
    scaleFontColor: 'red',
    data: {
      labels: testNames,
      datasets: ['hyperbuild-nodejs', 'html-minifier', 'minimize'].map(program => ({
        label: program,
        backgroundColor: colours[program],
        data: testNames.map(test => sizes[test][program].relative * 100),
      })),
    },
    ...chartOptions('Relative minified HTML file size (lower is better)', true, tick => `${tick}%`),
  });
})();
