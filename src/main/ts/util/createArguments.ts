export function createArguments (args: object): string[] {
  return Object.keys(args).map(k => {
    let rawArgs = [];

    let opt = `--${k}`;
    let val = args[k];

    if (typeof val == "boolean") {
      if (val) {
        rawArgs[0] = opt;
      }

    } else if (typeof val == "string" || typeof val == "number") {
      rawArgs[0] = opt;
      rawArgs[1] = `${val}`;

    } else if (Array.isArray(val)) {
      if (val.length) {
        rawArgs[0] = opt;
        rawArgs[1] = val.join(",");
      }

    } else {
      throw new TypeError(`Unknown argument value type for argument "${k}" with value: ${val}`);
    }

    return rawArgs;
  }).reduce((flat, c) => flat.concat(c), []);
}
