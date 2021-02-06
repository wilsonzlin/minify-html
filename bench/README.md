# Benchmarking

This folder contains scripts used to test the performance and effectiveness of minify-html, for guided optimisation and/or comparisons.

It also contains a set of common web pages as tests for benchmarking.

## Comparison

The [Node.js version of minify-html](../nodejs) is tested against [html-minfier](https://github.com/kangax/html-minifier) and [minimize](https://github.com/Swaagie/minimize) in two dimensions:

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

**Note that these pages are already mostly minified.**

For more information on how the tests are fetched, see [fetch.js](./fetch.js).

On this [project's README](../README.md), average graphs are shown. Graphs showing per-test results are shown below:

<img width="435" alt="Chart showing speed of HTML minifiers per test" src="https://wilsonl.in/minify-html/bench/0.4.3/core/speeds.png"> <img width="435" alt="Chart showing effectiveness of HTML minifiers per test" src="https://wilsonl.in/minify-html/bench/0.4.3/core/sizes.png">

Since speed depends on the input, speed charts show performance relative to the Node.js minify-html as a percentage.

The settings used for each minifier can be found in [minifiers.js](./minifiers.js). Some settings to note:

- All minifiers are configured to use esbuild for JS minification asynchronously and in parallel, similar to how minify-html works.
- `conservativeCollapse` is enabled for html-minifier as otherwise some whitespace would be unsafely removed with side effects. minify-html can safely remove whitespace with context if configured properly.

## Running

Make sure to install the dependencies listed in [package.json](./package.json) by running `npm i` or `yarn`. Node.js 10 is required, and system dependencies for building [canvas](https://www.npmjs.com/package/canvas), used for rendering graphs, may need to be installed. See the [npm package](https://www.npmjs.com/package/canvas) for more details.

Run [build.sh](./build.sh) to build @minify-html/js with the local minify-html.

Run [sizes.js](sizes.js) to run each HTML minifier against each test and record the minified size results. This will also output the minified files in `min` if inspection of minified outputs is necessary. [compare.sh](./compare.sh) is a useful script for viewing a character-by-character diff between the minified outputs of minify-html and html-minifier for a specific test. Pass the test's file name as the first argument.

Run [speeds.js](./speeds.js) to benchmark the performance of each HTML minifier against each test and record the op/s results.

Run [graph.js](./graph.js) to render graphs from recorded speed and size results in the `results` folder.

## minify-html-bench

The [minify-html-bench](./minify-html-bench) folder contains a Rust executable subproject that runs the local minify-html on all tests for many iterations to calculate speed as operations per second.

This can be useful for profiling the core code or checking the performance of minify-html in other languages with native bindings.

It takes two arguments:

- `--tests`: path to the folder containing tests to use as inputs.
- `--iterations`: how many iterations to run per test.

The results will be written to stdout as a JSON object, where properties are the test file names and values are the operations per second.

Profiling minify-html can be done on Linux by using [profile.sh](./profile.sh), which uses `perf`. The generated report can be used using `perf report`.
