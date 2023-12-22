const fs = require("fs");
const https = require("https");
const path = require("path");
const pkg = require("./package.json");
const crypto = require("crypto");
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

class InvalidChecksumError extends Error {}

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
  const hasher = crypto.createHash("sha512");
  hasher.update(fs.readFileSync(binaryPath));
  const gotHash = hasher.digest();
  const expectedHashHex = fs.readFileSync(path.join(__dirname, "checksum", `${binaryName}.sha512sum`), "utf8").trim();
  if (!/^[a-fA-F0-9]{128}$/.test(expectedHashHex)) {
    throw new Error(`Invalid expected hash: ${expectedHashHex}`);
  }
  const expectedHash = Buffer.from(expectedHashHex, "hex");
  if (!gotHash.equals(expectedHash)) {
    throw new InvalidChecksumError(`WARNING: Downloaded binary does not match expected hash. This may be caused by a temporary network or server error, programming bug, or malicious activity.`);
  }
};

if (
  !fs.existsSync(path.join(__dirname, ".no-postinstall")) &&
  !fs.existsSync(binaryPath)
) {
  downloadNativeBinary().then(
    () => console.log(`Downloaded ${pkg.name}`),
    (err) => {
      if (err instanceof InvalidChecksumError) {
        console.error(err.message);
        return;
      }
      console.error(
        `Failed to download ${pkg.name}, will build from source: ${err}`
      );
      fs.writeFileSync(
        `${__dirname}/Cargo.toml`,
        fs.readFileSync(`${__dirname}/Cargo.toml`, "utf8").replace(/^minify-html = \{.*$/m, () => `minify-html = "${pkg.version}"`),
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
