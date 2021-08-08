const fs = require("fs");
const path = require("path");

const RESULTS_DIR = path.join(__dirname, "results");
const INPUTS_DIR = path.join(__dirname, "inputs");

module.exports = {
  calculate: () => {
    // minifier => avg(ops).
    const minifierAvgOps = {};
    // minifier => avg(1 - output / original).
    const minifierAvgReduction = {};
    let maxMinifierAvgOps = 0;
    // minifier => input => ops.
    const perInputOps = {};
    // minifier => input => (1 - output / original).
    const perInputReduction = {};
    // input => max(ops).
    const maxInputOps = {};
    const inputSizes = Object.fromEntries(
      fs.readdirSync(INPUTS_DIR).map((f) => {
        const name = path.basename(f, ".json");
        const stats = fs.statSync(path.join(INPUTS_DIR, f));
        return [name, stats.size];
      })
    );

    for (const f of fs.readdirSync(RESULTS_DIR)) {
      const minifier = decodeURIComponent(path.basename(f, ".json"));
      const data = JSON.parse(
        fs.readFileSync(path.join(RESULTS_DIR, f), "utf8")
      );
      for (const [input, size, iterations, seconds] of data) {
        const originalSize = inputSizes[input];
        const ops = 1 / (seconds / iterations);
        const reduction = 1 - size / originalSize;
        (minifierAvgOps[minifier] ??= []).push(ops);
        (minifierAvgReduction[minifier] ??= []).push(reduction);
        (perInputOps[minifier] ??= {})[input] = ops;
        (perInputReduction[minifier] ??= {})[input] = reduction;
        maxInputOps[input] = Math.max(maxInputOps[input] ?? 0, ops);
      }
    }

    const minifiers = Object.keys(minifierAvgOps);
    for (const m of minifiers) {
      minifierAvgOps[m] =
        minifierAvgOps[m].reduce((sum, ops) => sum + ops, 0) /
        minifierAvgOps[m].length;
      maxMinifierAvgOps = Math.max(maxMinifierAvgOps, minifierAvgOps[m]);
      minifierAvgReduction[m] =
        minifierAvgReduction[m].reduce((sum, ops) => sum + ops, 0) /
        minifierAvgReduction[m].length;
    }

    return {
      minifierAvgReduction,
      minifierAvgOps,
      maxMinifierAvgOps,
      perInputOps,
      perInputReduction,
      maxInputOps,
      inputSizes,
      minifiers,
    };
  },
};
