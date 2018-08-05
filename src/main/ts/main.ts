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
  in?: string;
  out?: string;

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

export type IHyperbuildMessage = IHyperbuildUnknownMessage | IHyperbuildDebugMessage | IHyperbuildInfoMessage | IHyperbuildWarnMessage | IHyperbuildFatalMessage;

export interface IHyperbuildUnknownMessage {
  type: "UNKNOWN";
  message: string;
}

export interface IHyperbuildDebugMessage {
  type: "DEBUG";
  message: string;
}

export interface IHyperbuildInfoMessage {
  type: "INFO";
  message: string;
}

export interface IHyperbuildWarnMessage {
  type: "WARN";
  message: string;
}

export interface IHyperbuildFatalMessage {
  type: "FATAL";
  message: string;
}

export class HyperbuildError extends Error {
  code: number;
  constructor(status: number, message: string) {
    super();
    this.code = status;
    this.message = `hyperbuild exited with status ${status} and encountered the following errors:\n\n${message}`;
  }
}

export interface IHyperbuildResult {
  code?: string;
  messages: {
    unknown: IHyperbuildUnknownMessage[];
    debug: IHyperbuildDebugMessage[];
    info: IHyperbuildInfoMessage[];
  };
  warnings: IHyperbuildWarnMessage[];
}

function processHyperbuildMessages(stderrOutput: string): IHyperbuildMessage[] {
  let messages: IHyperbuildMessage[] = [];

  // Be careful to keep messages intact, including keeping CR, LF, and CRLF differences
  let regex = /.*[\r\n]/g;
  let match;

  while (match = regex.exec(stderrOutput)) {
    let line = strip_ansi(match[0]);
    let prefixMatch = /^\[([A-Z]+)] /.exec(line);

    if (prefixMatch) {
      // Don't trim: it's possible trailing newline is part of message,
      // and if message continues past newline, the newline is intentional
      messages.push({
        type: prefixMatch[1],
        message: line.slice(prefixMatch[0].length),
      } as IHyperbuildMessage);

    } else {
      // Assume if no prefix, line is continuation of previous message
      if (!messages.length) {
        messages.push({
          type: "UNKNOWN",
          message: line,
        });
      } else {
        messages[messages.length - 1].message += line;
      }
    }
  }

  return messages;
}

const BIN_PATH = __dirname + "/hyperbuild";

function escapeOptionValue(val: string): string {
  return `'${val.replace(/'/g, "\\'")}'`;
}

function stringifyToArguments(args: object): string {
  return Object.keys(args).map(k => {
    let a = `--${k}`;
    let v = args[k];

    if (typeof v == "boolean") {
      return v ? a : null;

    } else if (typeof v == "string" || typeof v == "number") {
      return `${a} ${escapeOptionValue(`${v}`)}`;

    } else if (Array.isArray(v)) {
      return !v.length ? null : `${a} ${escapeOptionValue(v.join(","))}`;

    } else {
      throw new TypeError(`Unknown argument value type for argument "${k}" with value: ${v}`);
    }
  }).join(" ");
}

export function hyperbuild(settings: IHyperbuildSettings): Promise<IHyperbuildResult> {
  return new Promise((resolve, reject) => {
    let { showLogging, code, ...argsRaw } = settings;
    let args = stringifyToArguments(argsRaw);

    let cmd = `${BIN_PATH} ${args}`;

    let proc = shelljs.exec(cmd, {
      async: true,
      silent: true,
    }, (status, stdout, stderr) => {
      if (showLogging) {
        console.error(stderr);
      }

      let result: IHyperbuildResult = {
        code: code == undefined ? undefined : stdout,
        messages: {
          unknown: [],
          debug: [],
          info: [],
        },
        warnings: [],
      }

      for (let message of processHyperbuildMessages(stderr)) {
        switch (message.type) {
          case "FATAL":
            reject(new HyperbuildError(status, message.message));
            return;

          case "UNKNOWN":
            result.messages.unknown.push(message);
            break;

          case "DEBUG":
            result.messages.debug.push(message);
            break;

          case "INFO":
            result.messages.info.push(message);
            break;

          case "WARN":
            result.warnings.push(message);
            break;
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
