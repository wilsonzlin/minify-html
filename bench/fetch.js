const {promises: fs} = require('fs');
const request = require('request-promise-native');
const path = require('path');
const prettier = require('prettier');

const tests = {
  "Amazon": "https://amazon.com/",
  "Bootstrap": "https://getbootstrap.com/docs/3.4/css/",
  "Bing": "https://bing.com/",
  "Coding Horror": "https://blog.codinghorror.com/",
  "ECMA-262": "https://tc39.es/ecma262/",
  "Google": "https://google.com/",
  "Hacker News": "https://news.ycombinator.com/",
  "NY Times": "https://nytimes.com/",
  "Reddit": "https://reddit.com/",
  "Twitter": "https://twitter.com/",
  "Wikipedia": "https://en.wikipedia.org/wiki/Soil",
};

const fetchTest = async (name, url) => {
  const html = await request({
    url,
    headers: {
      'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; rv:71.0) Gecko/20100101 Firefox/71.0',
    },
  });
  console.log(`Fetched ${name}`);
  return [name, html];
};

(async () => {
  const existing = await fs.readdir(path.join(__dirname, 'tests'));
  await Promise.all(existing.map(e => fs.unlink(path.join(__dirname, 'tests', e))));

  // Format after fetching as formatting is synchronous and can take so long that connections get dropped by server due to inactivity.
  for (const [name, html] of await Promise.all(Object.entries(tests).map(([name, url]) => fetchTest(name, url)))) {
    const formatted = prettier.format(html, {parser: 'html'});
    console.log(`Formatted ${name}`);
    await fs.writeFile(path.join(__dirname, 'tests', `${name}.html`), formatted);
  }
})()
  .catch(console.error);
