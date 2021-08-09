const fs = require("fs/promises");
const https = require("https");
const path = require("path");
const results = require("./results");

const GRAPHS_DIR = path.join(__dirname, "graphs");
const SPEEDS_GRAPH = path.join(GRAPHS_DIR, "speeds.png");
const SIZES_GRAPH = path.join(GRAPHS_DIR, "sizes.png");
const AVERAGE_SPEEDS_GRAPH = path.join(GRAPHS_DIR, "average-speeds.png");
const AVERAGE_SIZES_GRAPH = path.join(GRAPHS_DIR, "average-sizes.png");

const speedColours = {
  "minify-html": "#2e61bd",
  "minify-html-onepass": "#222",
};
const defaultSpeedColour = "rgb(188, 188, 188)";

const sizeColours = {
  "minify-html": "#2e61bd",
};
const defaultSizeColour = "rgb(188, 188, 188)";

const averageChartOptions = (label) => ({
  options: {
    legend: {
      display: false,
    },
    scales: {
      xAxes: [
        {
          barPercentage: 0.5,
          gridLines: {
            display: false,
          },
          ticks: {
            fontColor: "#555",
            fontSize: 20,
          },
        },
      ],
      yAxes: [
        {
          type: "linear",
          scaleLabel: {
            display: true,
            fontColor: "#222",
            fontSize: 24,
            fontStyle: "bold",
            labelString: label,
            padding: 12,
          },
          position: "left",
          ticks: {
            callback: "$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$",
            fontColor: "#222",
            fontSize: 20,
          },
          gridLines: {
            color: "#eee",
          },
        },
      ],
    },
  },
});

const breakdownChartOptions = (title) => ({
  options: {
    legend: {
      display: true,
      labels: {
        fontColor: "#000",
        fontSize: 20,
      },
    },
    title: {
      display: true,
      text: title,
      fontColor: "#333",
      fontSize: 24,
    },
    scales: {
      xAxes: [
        {
          gridLines: {
            color: "#f2f2f2",
          },
          ticks: {
            callback: "$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$",
            fontColor: "#999",
            fontSize: 20,
          },
        },
      ],
      yAxes: [
        {
          barPercentage: 0.5,
          gridLines: {
            color: "#aaa",
          },
          ticks: {
            fontColor: "#666",
            fontSize: 20,
          },
        },
      ],
    },
  },
});

const renderChart = (cfg, width, height) =>
  new Promise((resolve, reject) => {
    const req = https.request("https://quickchart.io/chart", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
    });
    req.on("error", reject);
    req.on("response", (res) => {
      const err = res.headers["x-quickchart-error"];
      if (res.statusCode < 200 || res.statusCode > 299 || err) {
        return reject(new Error(err || `Status ${res.statusCode}`));
      }
      const chunks = [];
      res.on("error", reject);
      res.on("data", (c) => chunks.push(c));
      res.on("end", () => resolve(Buffer.concat(chunks)));
    });
    req.end(
      JSON.stringify({
        backgroundColor: "white",
        chart: JSON.stringify(cfg).replaceAll(
          '"$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$"',
          "function(value) {return Math.round(value * 10000) / 100 + '%';}"
        ),
        width,
        height,
        format: "png",
      })
    );
  });

(async () => {
  await fs.mkdir(GRAPHS_DIR, { recursive: true });

  const res = results.calculate();
  const speedMinifiers = [
    "html-minifier",
    "minimize",
    "minify-html",
    "minify-html-onepass",
  ];
  const sizeMinifiers = ["minimize", "html-minifier", "minify-html"];
  const inputs = Object.keys(res.inputSizes).sort();

  await fs.writeFile(
    AVERAGE_SPEEDS_GRAPH,
    await renderChart(
      {
        type: "bar",
        data: {
          labels: speedMinifiers,
          datasets: [
            {
              backgroundColor: speedMinifiers.map(
                (n) => speedColours[n] ?? defaultSpeedColour
              ),
              data: speedMinifiers.map(
                (m) => res.minifierAvgOps[m] / res.maxMinifierAvgOps
              ),
            },
          ],
        },
        ...averageChartOptions("Performance"),
      },
      1024,
      768
    )
  );

  await fs.writeFile(
    AVERAGE_SIZES_GRAPH,
    await renderChart(
      {
        type: "bar",
        data: {
          labels: sizeMinifiers,
          datasets: [
            {
              backgroundColor: sizeMinifiers.map(
                (n) => sizeColours[n] ?? defaultSizeColour
              ),
              data: sizeMinifiers.map((m) => res.minifierAvgReduction[m]),
            },
          ],
        },
        ...averageChartOptions("Reduction"),
      },
      1024,
      768
    )
  );

  await fs.writeFile(
    SPEEDS_GRAPH,
    await renderChart(
      {
        type: "horizontalBar",
        data: {
          labels: inputs,
          datasets: speedMinifiers.map((minifier) => ({
            label: minifier,
            data: inputs.map(
              (input) =>
                res.perInputOps[minifier][input] /
                res.perInputOps["minify-html"][input]
            ),
          })),
        },
        ...breakdownChartOptions(
          "Operations per second, relative to minify-html"
        ),
      },
      900,
      1600
    )
  );

  await fs.writeFile(
    SIZES_GRAPH,
    await renderChart(
      {
        type: "horizontalBar",
        data: {
          labels: inputs,
          datasets: sizeMinifiers.map((minifier) => ({
            label: minifier,
            data: inputs.map(
              (input) =>
                res.perInputReduction[minifier][input] /
                res.perInputReduction["minify-html"][input]
            ),
          })),
        },
        ...breakdownChartOptions("Size reduction, relative to minify-html"),
      },
      900,
      1600
    )
  );
})();
