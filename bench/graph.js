const results = require('./results');
const https = require('https');

const colours = {
  'minify-html': '#041f60',
  '@minify-html/js': '#1f77b4',
  'minimize': '#ff7f0e',
  'html-minifier': '#2ca02c',
};

const COLOUR_SPEED_PRIMARY = '#2e61bd';
const COLOUR_SPEED_SECONDARY = 'rgb(188, 188, 188)';
const COLOUR_SIZE_PRIMARY = '#64acce';
const COLOUR_SIZE_SECONDARY = 'rgb(224, 224, 224)';

const breakdownChartOptions = (title) => ({
  options: {
    legend: {
      display: true,
      labels: {
        fontColor: '#000',
      },
    },
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
  },
});

const combinedChartOptions = () => ({
  options: {
    legend: {
      display: false,
    },
    scales: {
      xAxes: [
        {
          gridLines: {
            display: false,
          },
          ticks: {
            fontColor: '#555',
            fontSize: 20,
          },
        },
      ],
      yAxes: [
        {
          id: 'y1',
          type: 'linear',
          scaleLabel: {display: true, fontSize: 22, fontColor: COLOUR_SPEED_PRIMARY, fontStyle: 'bold', labelString: 'Average performance'},
          position: 'left',
          ticks: {
            callback: "$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$",
            fontColor: COLOUR_SPEED_PRIMARY,
            fontSize: 16,
          },
          gridLines: {
            color: '#eee',
          },
        },
        {
          id: 'y2',
          type: 'linear',
          scaleLabel: {display: true, fontSize: 22, fontColor: COLOUR_SIZE_PRIMARY, fontStyle: 'bold', labelString: 'Average size reduction'},
          position: 'right',
          ticks: {
            callback: "$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$",
            fontColor: COLOUR_SIZE_PRIMARY,
            fontSize: 16,
          },
          gridLines: {
            display: false,
          }
        },
      ],
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
    backgroundColor: 'white',
    chart: JSON.stringify(cfg).replaceAll('"$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$"', "function(value) {return Math.round(value * 10000) / 100 + '%';}"),
    width: 900,
    height: 650,
    format: 'png',
  }));
});

(async () => {
  const averageSpeeds = results.getSpeedResults().getAverageRelativeSpeedPerMinifier('@minify-html/js');
  const averageSizes = results.getSizeResults().getAverageRelativeSizePerMinifier();
  const averageLabels = ["html-minifier", "minimize", "@minify-html/js"];

  results.writeAverageCombinedGraph(await renderChart({
    type: 'bar',
    data: {
      labels: averageLabels,
      datasets: [
        {
          yAxisID: 'y1',
          backgroundColor: averageLabels.map((n) => n === "@minify-html/js" ? COLOUR_SPEED_PRIMARY : COLOUR_SPEED_SECONDARY),
          data: averageLabels.map((n) => averageSpeeds.get(n)),
        },
        {
          yAxisID: 'y2',
          backgroundColor: averageLabels.map((n) => n === "@minify-html/js" ? COLOUR_SIZE_PRIMARY : COLOUR_SIZE_SECONDARY),
          data: averageLabels.map((n) => 1 - averageSizes.get(n)),
        },
      ],
    },
    ...combinedChartOptions(),
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
    ...breakdownChartOptions('Operations per second (higher is better)'),
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
    ...breakdownChartOptions('Minified size (lower is better)'),
  }));
})();
