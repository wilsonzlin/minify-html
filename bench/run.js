const fs = require('fs');
const mkdirp = require('mkdirp');
const path = require('path');
const minifiers = require('./minifiers');
const tests = require('./tests');

for (const t of tests) {
  for (const m of Object.keys(minifiers)) {
    try {
      const minPath = path.join(__dirname, 'min', m, `${t.name}.html`);
      mkdirp.sync(path.dirname(minPath));
      fs.writeFileSync(minPath, minifiers[m](t.contentAsString, t.contentAsBuffer));
    } catch (err) {
      console.error(`Failed to run ${m} on test ${t.name}:`);
      console.error(err);
      process.exit(1);
    }
  }
}
