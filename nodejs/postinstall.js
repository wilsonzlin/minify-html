const fs = require("fs");
const https = require("https");
const path = require("path");
const zlib = require("zlib");
const pkg = require("./package.json");

const MAX_DOWNLOAD_ATTEMPTS = 4;
const BINARY_CMD = "minify-html";

const { platform, arch } = process;
const isWindows = platform === "win32";
const isLinux = platform === "linux";
const isMacOS = platform === "darwin";
const binaryPaths = {
  nodejs: path.join(__dirname, "index.node"),
  cli: path.join(__dirname, "cli"),
};

const buildUrl = (target) => {
  let url = 'https://wilsonl.in/minify-html/bin/';

  if (target === 'nodejs') {
    const binaryName = [platform, arch].join("__");
    const file = `${binaryName}.node.gz`;

    url += `/${target}/${pkg.version}/${pkg.name.split("/")[1]}/${file}`;
  }
  else if (target === 'cli') {
    const os = isWindows
      ? 'windows'
      : isLinux
      ? 'linux'
      : isMacOS
      ? 'macos'
      : null;

    if (os === null) {
      console.warn(`[${pkg.name} CLI] Your OS (${os}) doesn\'t seem ` +
        `to be supported yet. Running ${BINARY_CMD} might fail.`);
    }

    const fileExt = os === 'windows' ? '.exe' : '';
    const fallbackArch = /x(?:86_)?64/.test(arch) ? 'x86_64' : null;

    if (!fallbackArch) {
      console.warn(`[${pkg.name} CLI] Your CPU architecture (${arch}) ` +
        `is not supported yet. Running ${BINARY_CMD} might fail.`);
    }
    else if (fallbackArch !== arch) {
      console.info(`[${pkg.name} CLI] Using ${fallbackArch} binary ` +
        `as fallback for your CPU architecture (${arch}).`);
    }

    const file = `${pkg.version}-${os}-${fallbackArch}${fileExt}`;

    url += `/${file}`;
  }
  else {
    throw new RangeError(`${target} must be "nodejs", or "cli".`);
  }

  return url;
};
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
        return reject(new StatusError(resp.statusCode));
      }
      const parts = [];
      resp.on("data", (chunk) => parts.push(chunk));
      resp.on("end", () => resolve(Buffer.concat(parts)));
    });
    stream.on("error", reject);
  });

const downloadNativeBinary = async (url, destination, unzip) => {
  for (let attempt = 0; ; attempt++) {
    let binary;
    try {
      binary = await fetch(url);
    } catch (e) {
      if (e instanceof StatusError && attempt < MAX_DOWNLOAD_ATTEMPTS) {
        await wait(Math.random() * 2500 + 500);
        continue;
      }
      throw e;
    }

    const fileContent = unzip
      ? zlib.gunzipSync(binary)
      : binary;

    fs.writeFileSync(destination, fileContent);
    break;
  }
};

/**
 * Fetch multiple binaries at the same time.
 * 
 * @TODO  Consider downloading one file after another to prevent TOO_MANY_REQUESTS
 *        server errors (if applicable).
 */
Promise.all(
  Object.keys(binaryPaths).map(async (target) => {
    const binaryPath = binaryPaths[target];

    if (
      !fs.existsSync(path.join(__dirname, ".no-postinstall")) &&
      !fs.existsSync(binaryPath)
    ) {
      const url = buildUrl(target);
      const isTargetingNodejs = target === 'nodejs';
      const unzip = isTargetingNodejs;

      return downloadNativeBinary(url, binaryPath, unzip).then(
        () => console.log(`Downloaded the ${target} version of ${pkg.name}`),
        (err) => {
          console.error(`Failed to download the ${target} version of ${pkg.name}: ${err}`);
          // Don't exit if the binary for the CLI is not downloaded.
          if (isTargetingNodejs) {
            process.exit(1);
          }
        }
      );
    }
  })
).then(() => {
  console.log(`${pkg.name} was downloaded successfully.`);
}).catch((err) => {
  console.error(err);
});
