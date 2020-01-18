const benchmark = require('benchmark');
const childProcess = require('child_process');
const minimist = require('minimist');
const path = require('path');
const minifiers = require('./minifiers');
const results = require('./results');
const tests = require('./tests');

const args = minimist(process.argv.slice(2));
const shouldRunRust = !!args.rust;

const cmd = (command, ...args) => {
  const throwErr = msg => {
    throw new Error(`${msg}\n  ${command} ${args.join(' ')}`);
  };

  const {status, signal, error, stdout, stderr} = childProcess.spawnSync(command, args.map(String), {
    stdio: ['ignore', 'pipe', 'pipe'],
    encoding: 'utf8',
  });
  if (error) {
    throwErr(error.message);
  }
  if (signal) {
    throwErr(`Command exited with signal ${signal}`);
  }
  if (status !== 0) {
    throwErr(`Command exited with status ${status}`);
  }
  if (stderr) {
    throwErr(`stderr: ${stderr}`);
  }
  return stdout;
};

const fromEntries = entries => {
  if (Object.fromEntries) return Object.fromEntries(entries);
  const obj = {};
  for (const [prop, val] of entries) obj[prop] = val;
  return obj;
};

const sizes = {};
const setSize = (program, test, result) => {
  if (!sizes[test]) {
    sizes[test] = {
      original: {
        absolute: tests.find(t => t.name === test).contentAsString.length,
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
  for (const m of Object.keys(minifiers)) {
    try {
      setSize(m, t.name, minifiers[m](t.contentAsString, t.contentAsBuffer).length);
    } catch (err) {
      console.error(`Failed to run ${m} on test ${t.name}:`);
      console.error(err);
      process.exit(1);
    }
  }
}
results.writeSizeResults(sizes);

const runTest = test => new Promise((resolve, reject) => {
  // Run JS libraries.
  const suite = new benchmark.Suite();
  for (const m of Object.keys(minifiers)) {
    suite.add(m, () => {
      minifiers[m](test.contentAsString, test.contentAsBuffer);
    });
  }
  suite
    .on('cycle', event => console.info(test.name, event.target.toString()))
    .on('complete', () => resolve(fromEntries(suite.map(b => [b.name, b.hz]))))
    .on('error', reject)
    .run({'async': true});
});

(async () => {
  const speeds = fromEntries(tests.map(t => [t.name, {}]));

  // Run Rust library.
  if (shouldRunRust) {
    for (const [testName, testOps] of JSON.parse(cmd(
      path.join(__dirname, 'hyperbuild-bench', 'target', 'release', 'hyperbuild-bench'),
      '--iterations', 512,
      '--tests', path.join(__dirname, 'tests'),
    ))) {
      Object.assign(speeds[testName], {hyperbuild: testOps});
    }
  }

  for (const t of tests) {
    Object.assign(speeds[t.name], await runTest(t));
  }
  results.writeSpeedResults(speeds);
})();
