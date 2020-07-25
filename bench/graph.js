const chartjs = require('chartjs-node');
const results = require('./results');

const colours = {
  'minify-html': '#041f60',
  '@minify-html/js': '#1f77b4',
  'minimize': '#ff7f0e',
  'html-minifier': '#2ca02c',
};

const percentageTick = t => (Math.round(t * 1000) / 10).toFixed(1) + '%';

const chartOptions = (title, displayLegend, yTick = t => t) => ({
  options: {
    title: {
      display: true,
      text: title,
      fontColor: '#333',
      fontSize: 24,
    },
    scales: {
      xAxes: [{
        barPercentage: 0.25,
        gridLines: {
          color: '#e2e2e2',
        },
        ticks: {
          fontColor: '#666',
          fontSize: 20,
        },
      }],
      yAxes: [{
        gridLines: {
          color: '#ccc',
        },
        ticks: {
          callback: yTick,
          fontColor: '#666',
          fontSize: 20,
        },
      }],
    },
    legend: {
      display: displayLegend,
      labels: {
        fontColor: '#000',
      },
    },
  },
});

const renderChart = async (cfg) => {
  const chart = new chartjs(900, 650);
  await chart.drawChart(cfg);
  return chart.getImageBuffer('image/png');
};

(async () => {
  const averageSpeeds = results.getSpeedResults().getAverageRelativeSpeedPerMinifier('@minify-html/js');
  results.writeAverageSpeedsGraph(await renderChart({
    type: 'bar',
    data: {
      labels: averageSpeeds.map(([n]) => n),
      datasets: [{
        label: 'Average relative op/s',
        backgroundColor: '#1f77b4',
        data: averageSpeeds.map(([_, v]) => v),
      }],
    },
    ...chartOptions('Average operations per second (higher is better)', false, percentageTick),
  }));

  const speeds = results.getSpeedResults().getRelativeFileSpeedsPerMinifier('@minify-html/js');
  results.writeSpeedsGraph(await renderChart({
    type: 'bar',
    data: {
      labels: speeds[0][1].map(([n]) => n),
      datasets: speeds.map(([minifier, fileSpeeds]) => ({
        label: minifier,
        backgroundColor: colours[minifier],
        data: fileSpeeds.map(([_, speed]) => speed),
      })),
    },
    ...chartOptions('Operations per second (higher is better)', true, percentageTick),
  }));

  const averageSizes = results.getSizeResults().getAverageRelativeSizePerMinifier();
  results.writeAverageSizesGraph(await renderChart({
    type: 'bar',
    scaleFontColor: 'red',
    data: {
      labels: averageSizes.map(([n]) => n),
      datasets: [{
        label: 'Average minified size',
        backgroundColor: '#2ca02c',
        data: averageSizes.map(([_, v]) => v),
      }],
    },
    ...chartOptions('Average minified size (lower is better)', false, percentageTick),
  }));

  const sizes = results.getSizeResults().getRelativeFileSizesPerMinifier();
  results.writeSizesGraph(await renderChart({
    type: 'bar',
    data: {
      labels: sizes[0][1].map(([n]) => n),
      datasets: sizes.map(([minifier, fileSizes]) => ({
        label: minifier,
        backgroundColor: colours[minifier],
        data: fileSizes.map(([_, size]) => size),
      })),
    },
    ...chartOptions('Minified size (lower is better)', true, percentageTick),
  }));
})();
