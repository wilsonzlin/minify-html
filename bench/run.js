const fs = require('fs');
const mkdirp = require('mkdirp');
const path = require('path');
const programs = require('./minifiers');
const tests = require('./tests');

for (const t of tests) {
  for (const p of Object.keys(programs)) {
    try {
      const minPath = path.join(__dirname, 'min', p, t.name);
      mkdirp.sync(path.dirname(minPath));
      fs.writeFileSync(minPath, programs[p](t.content));
    } catch (err) {
      console.error(`Failed to run ${p} on test ${t.name}:`);
      console.error(err);
      process.exit(1);
    }
  }
}
