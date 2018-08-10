import strip_ansi from "strip-ansi";

export type IHyperbuildMessage =
  IHyperbuildUnknownMessage
  | IHyperbuildDebugMessage
  | IHyperbuildInfoMessage
  | IHyperbuildWarnMessage
  | IHyperbuildFatalMessage;

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

export function processHyperbuildMessages (stderrOutput: string): IHyperbuildMessage[] {
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
      // e.g. Expected `=`, got `
      // ` (U+000A)
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

  messages.forEach(m => {
    m.message = m.message.trim(); // Safe to trim now, as no message should end with raw user content
  });

  return messages;
}
