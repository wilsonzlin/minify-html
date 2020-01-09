const benchmark = require('benchmark');
const childProcess = require('child_process');
const fs = require('fs');
const path = require('path');
const programs = require('./minifiers');
const tests = require('./tests');

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
fs.writeFileSync(path.join(__dirname, 'minification.json'), JSON.stringify(sizes, null, 2));

const runTest = test => new Promise((resolve, reject) => {
  // Run JS libraries.
  const suite = new benchmark.Suite();
  for (const p of Object.keys(programs)) {
    suite.add(p, () => {
      programs[p](test.content);
    });
  }
  suite
    .on('cycle', event => console.info(test.name, event.target.toString()))
    .on('complete', () => resolve(fromEntries(suite.map(b => [b.name, b.hz]))))
    .on('error', reject)
    .run({'async': true});
});

(async () => {
  const results = {};

  // Run Rust library.
  for (const [testName, testOps] of JSON.parse(cmd(
    path.join(__dirname, 'hyperbuild-bench', 'target', 'release', 'hyperbuild-bench'),
    '--iterations', 100,
    '--tests', path.join(__dirname, 'tests'),
  ))) {
    results[testName] = {hyperbuild: testOps};
  }

  for (const t of tests) {
    Object.assign(results[t.name], await runTest(t));
  }
  fs.writeFileSync(path.join(__dirname, 'speed.json'), JSON.stringify(results, null, 2));
})();
