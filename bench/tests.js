const fs = require('fs');
const path = require('path');

const testsDir = path.join(__dirname, 'tests');
module.exports = fs.readdirSync(testsDir).map(name => ({
  name,
  content: fs.readFileSync(path.join(testsDir, name), 'utf8'),
})).sort((a, b) => a.name.localeCompare(b.name));
