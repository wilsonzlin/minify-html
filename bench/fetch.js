const {promises: fs} = require('fs');
const request = require('request-promise-native');
const path = require('path');
const prettier = require('prettier');

const tests = {
  "Amazon": "https://www.amazon.com/",
  "BBC": "https://www.bbc.co.uk/",
  "Bootstrap": "https://getbootstrap.com/docs/3.4/css/",
  "Bing": "https://www.bing.com/",
  "Coding Horror": "https://blog.codinghorror.com/",
  "ECMA-262": "https://www.ecma-international.org/ecma-262/10.0/index.html",
  "Google": "https://www.google.com/",
  "Hacker News": "https://news.ycombinator.com/",
  "NY Times": "https://www.nytimes.com/",
  "Reddit": "https://www.reddit.com/",
  "Stack Overflow": "https://www.stackoverflow.com/",
  "Twitter": "https://twitter.com/",
  "Wikipedia": "https://en.wikipedia.org/wiki/Soil",
};

const fetchTest = async (name, url) => {
  const html = await request({
    url,
    gzip: true,
    headers: {
      'Accept': '*/*',
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
    // Apply some fixes to HTML to allow strict formatter to work.
    const formatted = prettier.format(
      html
        // Fix missing semicolon after entity in Amazon.
        .replace(/&#x200b/g, '&#x200b;')
        // Fix consecutive malformed entities in Amazon.
        .replace(/&& window.ue_sbl/g, '&amp&amp window.ue_sbl')
        .replace(/&&pf_rd_p/g, '&amp&amppf_rd_p')
        // Fix early termination of conditional comment in Amazon.
        .replace('--></style>\n<![endif]-->', '</style>\n<![endif]-->')
        // Fix closing of void tag in Amazon.
        .replace(/><\/hr>/g, '/>')
        // Fix extra '</div>' in BBC.
        .replace('</a></span></small></div></div></div></footer>', '</a></span></small></div></div></footer>')
        // Fix consecutive malformed entities in Google.
        .replace(/&&google.aft/g, '&amp&ampgoogle.aft')
        // Fix parser failing to parse unquoted attribute value starting with forward slash in Stack Overflow.
        .replace('action=/search', 'action="/search"')
        // Fix broken attribute value in Stack Overflow.
        .replace('height=151"', 'height="151"')
      ,
      {parser: 'html'},
    );
    console.log(`Formatted ${name}`);
    await fs.writeFile(path.join(__dirname, 'tests', `${name}.html`), formatted);
  }
})()
  .catch(console.error);
