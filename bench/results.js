const minifiers = require("./minifiers");
const tests = require("./tests");
const { join } = require("path");
const { mkdirSync, readFileSync, writeFileSync } = require("fs");

const RESULTS_DIR = join(__dirname, "results");
const SPEEDS_JSON = join(RESULTS_DIR, "speeds.json");
const SPEEDS_GRAPH = join(RESULTS_DIR, "speeds.png");
const AVERAGE_COMBINED_GRAPH = join(RESULTS_DIR, "average-combined.png");
const AVERAGE_SPEEDS_GRAPH = join(RESULTS_DIR, "average-speeds.png");
const SIZES_JSON = join(RESULTS_DIR, "sizes.json");
const SIZES_GRAPH = join(RESULTS_DIR, "sizes.png");
const AVERAGE_SIZES_GRAPH = join(RESULTS_DIR, "average-sizes.png");

const minifierNames = Object.keys(minifiers);
const testNames = tests.map((t) => t.name);

mkdirSync(RESULTS_DIR, { recursive: true });

module.exports = {
  writeSpeedResults(speeds) {
    writeFileSync(SPEEDS_JSON, JSON.stringify(speeds, null, 2));
  },
  writeSizeResults(sizes) {
    writeFileSync(SIZES_JSON, JSON.stringify(sizes, null, 2));
  },
  writeAverageCombinedGraph(data) {
    writeFileSync(AVERAGE_COMBINED_GRAPH, data);
  },
  writeAverageSpeedsGraph(data) {
    writeFileSync(AVERAGE_SPEEDS_GRAPH, data);
  },
  writeSpeedsGraph(data) {
    writeFileSync(SPEEDS_GRAPH, data);
  },
  writeAverageSizesGraph(data) {
    writeFileSync(AVERAGE_SIZES_GRAPH, data);
  },
  writeSizesGraph(data) {
    writeFileSync(SIZES_GRAPH, data);
  },
  getSpeedResults() {
    const data = JSON.parse(readFileSync(SPEEDS_JSON, "utf8"));

    return {
      // Get minifier-speed pairs.
      getAverageRelativeSpeedPerMinifier(baselineMinifier) {
        return new Map(
          minifierNames.map((minifier) => [
            minifier,
            testNames
              // Get operations per second for each test.
              .map(
                (test) => data[test][minifier] / data[test][baselineMinifier]
              )
              // Sum all test operations per second.
              .reduce((sum, c) => sum + c) /
              // Divide by tests count to get average operations per second.
              testNames.length,
          ])
        );
      },
      // Get minifier-speeds pairs.
      getRelativeFileSpeedsPerMinifier(baselineMinifier) {
        return minifierNames.map((minifier) => [
          minifier,
          testNames.map((test) => [
            test,
            data[test][minifier] / data[test][baselineMinifier],
          ]),
        ]);
      },
    };
  },
  getSizeResults() {
    const data = JSON.parse(readFileSync(SIZES_JSON, "utf8"));

    return {
      // Get minifier-size pairs.
      getAverageRelativeSizePerMinifier() {
        return new Map(
          minifierNames.map((minifier) => [
            minifier,
            testNames
              .map((test) => data[test][minifier].relative)
              .reduce((sum, c) => sum + c) / testNames.length,
          ])
        );
      },
      // Get minifier-sizes pairs.
      getRelativeFileSizesPerMinifier() {
        return minifierNames.map((minifier) => [
          minifier,
          testNames.map((test) => [test, data[test][minifier].relative]),
        ]);
      },
    };
  },
};
