const fs = require("fs");

const common = {
  version: "0.6.9",
  main: "index.node",
  types: "index.d.ts",
  files: ["postinstall.js", "index.d.ts"],
  scripts: {
    build: "node-gyp build && shx mv build/Release/index.node index.node",
    clean:
      "cd native && cargo clean && cd .. && node-gyp clean && node-gyp configure && shx rm -f index.node",
    postinstall: "node postinstall.js",
  },
  repository: {
    type: "git",
    url: "git+https://github.com/wilsonzlin/minify-html.git",
  },
  author: {
    email: "npm@wilsonl.in",
    name: "Wilson Lin",
    url: "https://wilsonl.in/",
  },
  license: "MIT",
  bugs: {
    url: "https://github.com/wilsonzlin/minify-html/issues",
  },
  engines: {
    node: ">= 8.6.0",
  },
  homepage: "https://github.com/wilsonzlin/minify-html#readme",
  devDependencies: {
    "@types/node": "^14.6.0",
    "node-gyp": "^7.0.0",
    shx: "^0.3.2",
  },
  keywords: ["compress", "compressor", "fast", "html", "minifier", "minify"],
};

const specifics = {
  core: {
    name: "@minify-html/core",
    description: "Extremely fast and smart HTML minifier",
    bin: {
      "minify-html-core": "./cli.js",
    },
  },
  js: {
    name: "@minify-html/js",
    description: "Extremely fast and smart HTML + JS + CSS minifier",
    bin: {
      "minify-html": "./cli.js",
    },
  },
}[process.argv[2]];

fs.writeFileSync(
  "package.json",
  JSON.stringify(
    {
      ...common,
      ...specifics,
    },
    null,
    2
  )
);
