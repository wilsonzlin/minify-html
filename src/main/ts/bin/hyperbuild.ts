import {IHyperbuildResult} from "../res/result";
import childProcess, {ChildProcess} from "child_process";
import {createArguments} from "../util/createArguments";
import {processHyperbuildMessages} from "../res/message";
import {HyperbuildError} from "../res/error";
import {isReadableStream} from "../util/isReadableStream";
import ReadableStream = NodeJS.ReadableStream;
import * as util from "util";

const BIN_PATH = __dirname + "/../hyperbuild";

export type HyperbuildCode = string | ReadableStream;

export interface IHyperbuildSettings {
  showLogging?: boolean;

  keep?: boolean;
  buffer?: boolean;
  verbose?: boolean;

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

export type HyperbuildProcessCallback = (err: HyperbuildError | null, res?: IHyperbuildResult) => any;

export function launchHyperbuild (settings: IHyperbuildSettings, callback: HyperbuildProcessCallback): ChildProcess {
  let {showLogging, ...argsRaw} = settings;
  let args = createArguments(argsRaw);

  let stdout = "";
  let stderr = "";

  let proc = childProcess.spawn(BIN_PATH, args, {
    windowsHide: true,
  });

  proc.stdin.on("error", (e: any) => {
    if (e.code === "EPIPE") {
      // An error occurred while writing, which most likely means the input was long,
      // and so was streamed in portions, and a hyperbuild error occurred before
      // everything could be written
    } else {
      throw e;
    }
  });

  proc.stdout.on("data", (data: Buffer) => {
    stdout += data.toString("utf8");
  });

  proc.stderr.on("data", (data: Buffer) => {
    stderr += data.toString("utf8");
  });

  proc.on("exit", status => {
    if (showLogging) {
      console.error(stderr);
    }

    let result: IHyperbuildResult = {
      code: settings.in != undefined ? undefined : stdout,
      messages: {
        unknown: [],
        debug: [],
        info: [],
      },
      warnings: [],
    };

    for (let message of processHyperbuildMessages(stderr)) {
      switch (message.type) {
      case "FATAL":
        callback(new HyperbuildError(status, message.message));
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

    callback(null, result);
  });

  return proc;
}

export function feedHyperbuild (process: ChildProcess, code: string | ReadableStream): void {
  if (isReadableStream(code)) {
    code.pipe(process.stdin);
  } else {
    process.stdin.end(code);
  }
}
