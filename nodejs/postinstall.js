const fs = require("fs");
const https = require("https");
const path = require("path");
const pkg = require("./package.json");
const cp = require("child_process");

const MAX_DOWNLOAD_ATTEMPTS = 4;

const binaryName = [process.platform, process.arch].join("__");
const binaryPath = path.join(__dirname, "index.node");

const wait = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class StatusError extends Error {
  constructor(status) {
    super(`Bad status of ${status}`);
    this.status = status;
  }
}

const fetch = (url) =>
  new Promise((resolve, reject) => {
    const stream = https.get(url, (resp) => {
      if (!resp.statusCode || resp.statusCode < 200 || resp.statusCode > 299) {
        reject(new StatusError(resp.statusCode));
        // Destroy stream to allow Node.js to exit.
        // Destroy after `reject` in case "error" handler is unintentionally triggered.
        resp.destroy();
        return;
      }
      const parts = [];
      resp.on("data", (chunk) => parts.push(chunk));
      resp.on("end", () => resolve(Buffer.concat(parts)));
    });
    stream.on("error", reject);
  });

const downloadNativeBinary = async () => {
  for (let attempt = 0; ; attempt++) {
    let binary;
    try {
      binary = await fetch(
        `https://wilsonl.in/minify-html/bin/nodejs/${pkg.version}/${binaryName}.node`
      );
    } catch (e) {
      if (
        e instanceof StatusError &&
        e.status !== 404 &&
        attempt < MAX_DOWNLOAD_ATTEMPTS
      ) {
        await wait(Math.random() * 2500 + 500);
        continue;
      }
      throw e;
    }

    fs.writeFileSync(binaryPath, binary);
    break;
  }
};

if (
  !fs.existsSync(path.join(__dirname, ".no-postinstall")) &&
  !fs.existsSync(binaryPath)
) {
  downloadNativeBinary().then(
    () => console.log(`Downloaded ${pkg.name}`),
    (err) => {
      console.error(
        `Failed to download ${pkg.name}, will build from source: ${err}`
      );
      const out = cp.spawnSync("npm", ["run", "build-release"], {
        cwd: __dirname,
        stdio: ["ignore", "inherit", "inherit"],
      });
      process.exitCode = out.exitCode;
      if (out.error) {
        throw out.error;
      }
    }
  );
}
