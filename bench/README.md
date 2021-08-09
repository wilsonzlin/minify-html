# Benchmarking

This folder contains scripts used to test the performance and effectiveness of minify-html, for guided optimisation and/or comparisons.

It also contains a set of common web pages as tests for benchmarking.

## Comparison

Each minifier is run against each file in the [inputs](./inputs) folder, which are HTML pages fetched from popular websites:

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

<img width="435" alt="Chart showing speed of HTML minifiers per test" src="https://wilsonl.in/minify-html/bench/0.6.1/core/speeds.png"><img width="435" alt="Chart showing effectiveness of HTML minifiers per test" src="https://wilsonl.in/minify-html/bench/0.6.1/core/sizes.png">

Results depend on the input, so charts show performance relative to minify-html as a percentage.

## Running

Run [build](./build) to build the minifiers.

Run [run](./run) to benchmark the performance of each HTML minifier against each test and record the op/s results.

Run [graph.js](./graph.js) to render graphs from recorded speed and size results in the `results` folder.
