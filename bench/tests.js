const fs = require('fs');
const path = require('path');

const testsDir = path.join(__dirname, 'tests');
module.exports = fs.readdirSync(testsDir).map(name => ({
  name,
  contentAsString: fs.readFileSync(path.join(testsDir, name), 'utf8'),
  contentAsBuffer: fs.readFileSync(path.join(testsDir, name)),
})).sort((a, b) => a.name.localeCompare(b.name));
