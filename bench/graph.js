const results = require("./results");
const https = require("https");
const path = require("path");
const fs = require("fs/promises");

const GRAPHS_DIR = path.join(__dirname, "graphs");
const SPEEDS_GRAPH = path.join(GRAPHS_DIR, "speeds.png");
const SIZES_GRAPH = path.join(GRAPHS_DIR, "sizes.png");
const AVERAGE_SPEEDS_GRAPH = path.join(GRAPHS_DIR, "average-speeds.png");
const AVERAGE_SIZES_GRAPH = path.join(GRAPHS_DIR, "average-sizes.png");

const speedColours = {
  "@minify-html/js": "#2e61bd",
  "minify-html": "#2e61bd",
  "minify-html-onepass": "#222",
};
const defaultSpeedColour = "rgb(188, 188, 188)";

const sizeColours = {
  "minify-html": "#2e61bd",
};
const defaultSizeColour = "rgb(188, 188, 188)";

const breakdownChartOptions = (title) => ({
  options: {
    legend: {
      display: true,
      labels: {
        fontColor: "#000",
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

const axisLabel = (fontColor, labelString) => ({
  display: true,
  fontColor,
  fontSize: 24,
  fontStyle: "bold",
  labelString,
  padding: 12,
});

const averageChartOptions = (label) => ({
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
            fontColor: "#555",
            fontSize: 16,
          },
        },
      ],
      yAxes: [
        {
          type: "linear",
          scaleLabel: axisLabel("#222", label),
          position: "left",
          ticks: {
            callback: "$$$_____REPLACE_WITH_TICK_CALLBACK_____$$$",
            fontColor: "#222",
            fontSize: 16,
          },
          gridLines: {
            color: "#eee",
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
  const speedMinifiers = [...res.minifiers].sort(
    (a, b) => res.minifierAvgOps[a] - res.minifierAvgOps[b]
  );
  const sizeMinifiers = ["minimize", "html-minifier", "minify-html"];
  const inputs = Object.keys(res.inputSizes).sort();

  await fs.writeFile(
    AVERAGE_SPEEDS_GRAPH,
    await renderChart(
      {
        type: "bar",
        data: {
          labels: speedMinifiers.map((m) => m.replace(" (", "\n(")),
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
          labels: sizeMinifiers.map((m) => m.replace(" (", "\n(")),
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
                res.perInputOps[minifier][input] / res.maxInputOps[input]
            ),
          })),
        },
        ...breakdownChartOptions("Operations per second (higher is better)"),
      },
      900,
      1000
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
            data: inputs.map((input) => res.perInputReduction[minifier][input]),
          })),
        },
        ...breakdownChartOptions("Size reduction (higher is better)"),
      },
      900,
      1000
    )
  );
})();
