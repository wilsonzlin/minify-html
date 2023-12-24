#!/usr/bin/env node

/*
  Normally I'd avoid scripts to generate code as it makes it harder to reason and work/develop with. But in this case the Java class is too tedious and error-prone to do manually.
*/

const fs = require("fs");

const cfgRs = fs.readFileSync(`${__dirname}/../../../../../../../minify-html/src/cfg/mod.rs`, "utf8");
const opts = [];
for (const [_, snake] of cfgRs.matchAll(/^\s*pub ([a-zA-Z0-9_]+): bool,?\s*$/gm)) {
  const pascal = snake.split("_").map((w) => `${w[0].toUpperCase()}${w.slice(1)}`).join("");
  opts.push({snake, pascal});
}

const java = `
package in.wilsonl.minifyhtml;

// WARNING: Do not manually edit, use Configuration.java.gen.js.

/**
 * Class representing minification configuration.
 */
public class Configuration {
  ${opts.map(o => `public final boolean ${o.snake};`).join("\n  ")}

  private Configuration(
    ${opts.map(o => `boolean ${o.snake}`).join(",\n    ")}
  ) {
    ${opts.map(o => `this.${o.snake} = ${o.snake};`).join("\n    ")}
  }

  /**
   * Builder to help create configuration.
   */
  public static class Builder {
    ${opts.map(o => `private boolean ${o.snake} = false;`).join("\n    ")}

    ${opts.map(o => `public Builder set${o.pascal}(boolean v) {
      this.${o.snake} = v;
      return this;
    }`).join("\n    ")}

    public Configuration build() {
      return new Configuration(
        ${opts.map(o => `this.${o.snake}`).join(",\n        ")}
      );
    }
  }
}
`.trim();

fs.writeFileSync(`${__dirname}/Configuration.java`, java);
