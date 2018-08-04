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

export interface IHyperbuildResult {
  code?: string;
}

export interface IHyperbuildMessage {
  message: string;
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
  constructor (status: number, errors: HyperbuildFatalMessage[]) {
    super();
    this.code = status;
    this.message = `hyperbuild exited with status ${status} and encountered the following errors:\n\n${errors.map(e => e.message).join("\n\n")}`;
  }
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
      // NOTE: This breaks if the very first line is not a message
      messages[messages.length - 1].message += line;
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

    if (settings.suppress) {
      args.push(`--suppress ${escapeOptionValue(settings.suppress.join(","))}`);
    }

    if (settings.MXcollapseWhitespace) {
      args.push(
        `--MXcollapseWhitespace ${escapeOptionValue(settings.MXcollapseWhitespace.join(","))}`);
    }
    if (settings.MXdestroyWholeWhitespace) {
      args.push(
        `--MXdestroyWholeWhitespace ${escapeOptionValue(settings.MXdestroyWholeWhitespace.join(","))}`);
    }
    if (settings.MXtrimWhitespace) {
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

    let proc = shelljs.exec(`${BIN_PATH} ${args.join(" ")}`, {
      async: true,
      silent: true,
    }, (status, stdout, stderr) => {
      if (settings.showLogging) {
        console.error(stderr);
      }

      let messages = processHyperbuildMessages(stderr);
      let errors = messages.filter(m => m instanceof HyperbuildFatalMessage);
      if (errors.length) {
        reject(new HyperbuildError(status, errors));
        return;
      }

      if (code != undefined) {
        resolve({
          code: stdout,
        });
      } else {
        resolve({});
      }
    });

    if (code != undefined) {
      if (isReadableStream(code)) {
        code.pipe(proc.stdin);
      } else {
        proc.stdin.end(code);
      }
    }
  });
}
