"use strict";

const benchmark = require('benchmark');
const chartjs = require('chartjs-node');
const fs = require('fs');
const path = require('path');
const programs = require('./minifiers');
const tests = require('./tests');

const colours = [
  {
    backgroundColor: '#9ad0f5',
    borderColor: '#47aaec',
  },
  {
    backgroundColor: '#ffb0c1',
    borderColor: '#ff87a1',
  },
  {
    backgroundColor: '#a4dfdf',
    borderColor: '#4bc0c0',
  },
];
const chartOptions = (title, displayLegend, yTick = t => t) => ({
  options: {
    title: {
      display: true,
      text: title,
    },
    scales: {
      xAxes: [{
        barPercentage: 0.5,
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

const sizes = {};
const setSize = (program, test, result) => {
  if (!sizes[test]) {
    sizes[test] = {
      original: {
        absolute: tests.find(t => t.name === test).content.length,
        relative: 1,
      },
    };
  }
  const original = sizes[test].original.absolute;
  sizes[test][program] = {
    absolute: result,
    relative: result / original,
  };
};

// Run once to set sizes.
for (const t of tests) {
  for (const p of Object.keys(programs)) {
    try {
      setSize(p, t.name, programs[p](t.content).length);
    } catch (err) {
      console.error(`Failed to run ${p} on test ${t.name}:`);
      console.error(err);
      process.exit(1);
    }
  }
}

const suite = new benchmark.Suite();
for (const p of Object.keys(programs)) {
  suite.add(p, () => {
    for (const t of tests) {
      programs[p](t.content);
    }
  });
}
suite
  .on('cycle', event => {
    console.info(event.target.toString());
  })
  .on('complete', async function () {
    const speedResults = this.map(b => ({
      name: b.name,
      ops: b.hz,
    }));
    fs.writeFileSync(path.join(__dirname, "speed.json"), JSON.stringify(speedResults, null, 2));
    await renderChart('speed', {
      type: 'bar',
      data: {
        labels: speedResults.map(r => r.name),
        datasets: [{
          ...colours[0],
          data: speedResults.map(r => r.ops),
        }],
      },
      ...chartOptions('Operations per second (higher is better)', false),
    });

    const testNames = Object.keys(sizes);
    const programNames = Object.keys(programs);
    fs.writeFileSync(path.join(__dirname, "minification.json"), JSON.stringify(sizes, null, 2));
    await renderChart('minification', {
      type: 'bar',
      scaleFontColor: 'red',
      data: {
        labels: testNames,
        datasets: programNames.map((program, i) => ({
          label: program,
          ...colours[i],
          data: testNames.map(test => sizes[test][program].relative * 100),
        })),
      },
      ...chartOptions('Relative minified HTML file size (lower is better)', true, tick => `${tick}%`),
    });
  })
  .run({'async': true});
