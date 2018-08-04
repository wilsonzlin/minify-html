import * as shelljs from "shelljs";
import ReadableStream = NodeJS.ReadableStream;
import strip_ansi = require("strip-ansi");

function isReadableStream(thing: any): thing is ReadableStream {
  return !!thing && typeof thing.pipe == "function";
}

export interface IHyperbuildSettings {
  showLogging?: boolean;

  keep?: boolean;
  buffer?: boolean;
  verbose?: boolean;

  code?: string | ReadableStream;
  inputFile?: string;
  outputFile?: string;

  suppress?: string[];

  MXcollapseWhitespace?: string[];
  MXdestroyWholeWhitespace?: string[];
  MXtrimWhitespace?: string[];

  MXtrimClassAttr?: boolean;
  MXdecEnt?: boolean;
  MXcondComments?: boolean;
  MXattrQuotes?: boolean;
  MXcomments?: boolean;
  MXoptTags?: boolean;
  MXtagWS?: boolean;
}

export interface IHyperbuildMessage {
  message: string;
}

export class HyperbuildUnknownMessage implements IHyperbuildMessage {
  message: string;

  constructor (message: string) {
    this.message = message;
  }
}

export class HyperbuildDebugMessage implements IHyperbuildMessage {
  message: string;

  constructor (message: string) {
    this.message = message;
  }
}

export class HyperbuildInfoMessage implements IHyperbuildMessage {
  message: string;

  constructor (message: string) {
    this.message = message;
  }
}

export class HyperbuildWarnMessage implements IHyperbuildMessage {
  message: string;

  constructor (message: string) {
    this.message = message;
  }
}

export class HyperbuildFatalMessage implements IHyperbuildMessage {
  message: string;

  constructor (message: string) {
    this.message = message;
  }
}

export class HyperbuildError extends Error {
  code: number;
  constructor (status: number, error: HyperbuildFatalMessage) {
    super();
    this.code = status;
    this.message = `hyperbuild exited with status ${status} and encountered the following errors:\n\n${error.message}`;
  }
}

export interface IHyperbuildResult {
  code?: string;
  unknown: HyperbuildUnknownMessage[];
  debug: HyperbuildDebugMessage[];
  info: HyperbuildInfoMessage[];
  warnings: HyperbuildWarnMessage[];
}

const MESSAGE_PREFIX_TO_CLASS_MAP: { [prefix: string]: new (message: string) => IHyperbuildMessage } = {
  "DEBUG": HyperbuildDebugMessage,
  "INFO": HyperbuildInfoMessage,
  "WARN": HyperbuildWarnMessage,
  "FATAL": HyperbuildFatalMessage,
};

function processHyperbuildMessages (stderrOutput: string): IHyperbuildMessage[] {
  let messages: IHyperbuildMessage[] = [];

  // Be careful to keep messages intact, including keeping CR, LF, and CRLF differences
  let regex = /.*[\r\n]/g;
  let match;

  while (match = regex.exec(stderrOutput)) {
    let line = strip_ansi(match[0]);
    let prefixMatch = /^\[([A-Z]+)] /.exec(line);

    if (prefixMatch) {
      let cls = MESSAGE_PREFIX_TO_CLASS_MAP[prefixMatch[1]];
      // Don't trim: it's possible trailing newline is part of message,
      // and if message continues past newline, the newline is intentional
      messages.push(new cls(line.slice(prefixMatch[0].length)));

    } else {
      // Assume if no prefix, line is continuation of previous message
      if (!messages.length) {
        messages.push(new HyperbuildUnknownMessage(line));
      } else {
        messages[messages.length - 1].message += line;
      }
    }
  }

  return messages;
}

const BIN_PATH = __dirname + "/hyperbuild";

function escapeOptionValue (val: string): string {
  return `'${val.replace(/'/g, "\\'")}'`;
}

export function hyperbuild (settings: IHyperbuildSettings): Promise<IHyperbuildResult> {
  return new Promise((resolve, reject) => {
    let args = [];

    if (settings.keep) {
      args.push("--keep");
    }
    if (settings.buffer) {
      args.push("--buffer");
    }
    if (settings.verbose) {
      args.push("--verbose");
    }

    if (settings.inputFile) {
      args.push(`--in ${escapeOptionValue(settings.inputFile)}`);
      args.push(`--out ${escapeOptionValue(settings.inputFile)}`);
    }

    if (settings.suppress && settings.suppress.length) {
      args.push(`--suppress ${escapeOptionValue(settings.suppress.join(","))}`);
    }

    if (settings.MXcollapseWhitespace && settings.MXcollapseWhitespace.length) {
      args.push(
        `--MXcollapseWhitespace ${escapeOptionValue(settings.MXcollapseWhitespace.join(","))}`);
    }
    if (settings.MXdestroyWholeWhitespace && settings.MXdestroyWholeWhitespace.length) {
      args.push(
        `--MXdestroyWholeWhitespace ${escapeOptionValue(settings.MXdestroyWholeWhitespace.join(","))}`);
    }
    if (settings.MXtrimWhitespace && settings.MXtrimWhitespace.length) {
      args.push(
        `--MXtrimWhitespace ${escapeOptionValue(settings.MXtrimWhitespace.join(","))}`);
    }

    if (settings.MXtrimClassAttr) {
      args.push("--MXtrimClassAttr");
    }
    if (settings.MXdecEnt) {
      args.push("--MXdecEnt");
    }
    if (settings.MXcondComments) {
      args.push("--MXcondComments");
    }
    if (settings.MXattrQuotes) {
      args.push("--MXattrQuotes");
    }
    if (settings.MXcomments) {
      args.push("--MXcomments");
    }
    if (settings.MXoptTags) {
      args.push("--MXoptTags");
    }
    if (settings.MXtagWS) {
      args.push("--MXtagWS");
    }

    let code = settings.code;

    let cmd = `${BIN_PATH} ${args.join(" ")}`;

    let proc = shelljs.exec(cmd, {
      async: true,
      silent: true,
    }, (status, stdout, stderr) => {
      if (settings.showLogging) {
        console.error(stderr);
      }

      let result: IHyperbuildResult = {
        code: code == undefined ? undefined : stdout,
        unknown: [],
        debug: [],
        info: [],
        warnings: [],
      }

      for (let message of processHyperbuildMessages(stderr)) {
        if (message instanceof HyperbuildFatalMessage) {
          reject(new HyperbuildError(status, message));
          return;

        } else if (message instanceof HyperbuildUnknownMessage) {
          result.unknown.push(message);

        } else if (message instanceof HyperbuildDebugMessage) {
          result.debug.push(message);

        } else if (message instanceof HyperbuildInfoMessage) {
          result.info.push(message);

        } else if (message instanceof HyperbuildWarnMessage) {
          result.warnings.push(message);

        } else {
          throw new Error(`INTERR Unknown message type with message: ${message}`);
        }
      }

      resolve(result);
    });

    if (code != undefined) {
      if (isReadableStream(code)) {
        code.pipe(proc.stdin);
      } else {
        proc.stdin.on("error", (e: any) => {
          if (e.code === "EPIPE") {
            // An error occurred while writing, which most likely means the input was long,
            // and so was streamed in portions, and a hyperbuild error occurred before
            // everything could be written
          } else {
            reject(e);
          }
        });
        proc.stdin.end(code);
      }
    }
  });
}
