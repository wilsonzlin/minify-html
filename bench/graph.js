const results = require('./results');
const https = require('https');

const colours = {
  'minify-html': '#041f60',
  '@minify-html/js': '#1f77b4',
  'minimize': '#ff7f0e',
  'html-minifier': '#2ca02c',
};

const chartOptions = (title, displayLegend) => ({
  options: {
    title: {
      display: true,
      text: title,
      fontColor: '#333',
      fontSize: 24,
    },
    scales: {
      xAxes: [
        {
          barPercentage: 0.25,
          gridLines: {
            color: '#e2e2e2',
          },
          ticks: {
            fontColor: '#666',
            fontSize: 20,
          },
        },
      ],
      yAxes: [
        {
          gridLines: {
            color: '#ccc',
          },
          ticks: {
            fontColor: '#666',
            fontSize: 20,
          },
        },
      ],
    },
    legend: {
      display: displayLegend,
      labels: {
        fontColor: '#000',
      },
    },
  },
});

const renderChart = (cfg) => new Promise((resolve, reject) => {
  const req = https.request('https://quickchart.io/chart', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
  });
  req.on('error', reject);
  req.on('response', res => {
    const err = res.headers['x-quickchart-error'];
    if (res.statusCode < 200 || res.statusCode > 299 || err) {
      reject(new Error(err || `Status ${res.statusCode}`));
    }
    const chunks = [];
    res.on('error', reject);
    res.on('data', c => chunks.push(c));
    res.on('end', () => resolve(Buffer.concat(chunks)));
  });
  req.end(JSON.stringify({
    chart: cfg,
    width: 900,
    height: 650,
    format: 'png',
  }));
});

(async () => {
  const averageSpeeds = results.getSpeedResults().getAverageRelativeSpeedPerMinifier('@minify-html/js');
  results.writeAverageSpeedsGraph(await renderChart({
    type: 'bar',
    data: {
      labels: averageSpeeds.map(([n]) => n),
      datasets: [
        {
          label: 'Average relative op/s',
          backgroundColor: '#1f77b4',
          data: averageSpeeds.map(([_, v]) => v),
        },
      ],
    },
    ...chartOptions('Average operations per second (higher is better)', false),
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
    ...chartOptions('Operations per second (higher is better)', true),
  }));

  const averageSizes = results.getSizeResults().getAverageRelativeSizePerMinifier();
  results.writeAverageSizesGraph(await renderChart({
    type: 'bar',
    scaleFontColor: 'red',
    data: {
      labels: averageSizes.map(([n]) => n),
      datasets: [
        {
          label: 'Average minified size',
          backgroundColor: '#2ca02c',
          data: averageSizes.map(([_, v]) => v),
        },
      ],
    },
    ...chartOptions('Average minified size (lower is better)', false),
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
    ...chartOptions('Minified size (lower is better)', true),
  }));
})();
