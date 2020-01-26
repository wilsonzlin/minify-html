# Benchmarking

This folder contains scripts used to test the performance and effectiveness of hyperbuild, for guided optimisation and/or comparisons.

It also contains a set of common web pages as tests for benchmarking.

## Comparison

The [Node.js version of hyperbuild](../nodejs) is tested against [html-minfier](https://github.com/kangax/html-minifier) and [minimize](https://github.com/Swaagie/minimize) in two dimensions:

- Speed as operations per second.
- Minified file size compared to the original.

[Benchmark.js](https://benchmarkjs.com) is used to determine speed. Each minifier is run against each file in the [tests](./tests) folder, which are HTML pages fetched from popular websites:

|File name|URL|
|---|---|
|Amazon|https://www.amazon.com/|
|BBC|https://www.bbc.co.uk/|
|Bootstrap|https://getbootstrap.com/docs/3.4/css/|
|Bing|https://www.bing.com/|
|Coding Horror|https://blog.codinghorror.com/|
|ECMA-262|https://www.ecma-international.org/ecma-262/10.0/index.html|
|Google|https://www.google.com/|
|Hacker News|https://news.ycombinator.com/|
|NY Times|https://www.nytimes.com/|
|Reddit|https://www.reddit.com/|
|Stack Overflow|https://www.stackoverflow.com/|
|Twitter|https://twitter.com/|
|Wikipedia|https://en.wikipedia.org/wiki/Soil|

For more information on how the tests are fetched, see [fetch.js](./fetch.js).

The results are available in the [results](./results) folder. On this [project's README](../README.md), average graphs are shown. Graphs showing per-test results are shown below:

<img width="435" alt="Chart showing speed of HTML minifiers per test" src="https://wilsonl.in/hyperbuild/bench/0.0.36/speeds.png"> <img width="435" alt="Chart showing effectiveness of HTML minifiers per test" src="https://wilsonl.in/hyperbuild/bench/0.0.36/sizes.png">

Since speed depends on the input, speed charts show performance relative to hyperbuild-nodejs as a percentage.

The settings used for each minifier can be found in [minifiers.js](./minifiers.js). Some settings to note:

- JS and CSS minification is disabled for all, as hyperbuild is an HTML minifier and there are existing separate tools for [such](https://github.com/terser/terser) [tasks](https://github.com/jakubpawlowicz/clean-css).
- `conservativeCollapse` is enabled for html-minifier as otherwise some whitespace would be unsafely removed with side affects. hyperbuild can safely remove whitespace with context if configured properly.

## Running

Make sure to install the dependencies listed in [package.json](./package.json) by running `npm i` or `yarn`. Node.js 10 is required, and system dependencies for building [canvas](https://www.npmjs.com/package/canvas), used for rendering graphs, might need to be installed. See the [npm package](https://www.npmjs.com/package/canvas) for more details.

Run [build.sh](./build.sh) to build hyperbuild-nodejs with the local hyperbuild.

Run [run.js](./run.js) to run each HTML minifier against each test and record the minified size results. This will also output the minified files in [min](./min) if inspection of minified outputs is necessary. [compare.sh](./compare.sh) is a useful script for viewing a character-by-character diff between the minified outputs of hyperbuild and html-minifier for a specific test. Pass the test's file name as the first argument.

Run [bench.js](./bench.js) to benchmark the performance of each HTML minifier against each test and record the op/s results.

Run [graph.js](./graph.js) to render graphs from recorded speed and size results in the [results](./results) folder.

## hyperbuild-bench

The [hyperbuild-bench](./hyperbuild-bench) folder contains a Rust executable subproject that runs the local hyperbuild on all tests for many iterations to calculate speed as operations per second.

This can be useful for profiling the core code or checking the performance of hyperbuild in other languages with native bindings.

It takes two arguments:

- `--tests`: path to the folder containing tests to use as inputs.
- `--iterations`: how many iterations to run per test.

The results will be written to stdout as a JSON object, where properties are the test file names and values are the operations per second.

Profiling hyperbuild can be done on Linux by using `perf`, for example:

```bash
perf record -g hyperbuild-bench/target/release/hyperbuild-bench --tests tests --iterations 1000
perf report
```
