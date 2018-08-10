import {
  IHyperbuildDebugMessage,
  IHyperbuildInfoMessage,
  IHyperbuildUnknownMessage,
  IHyperbuildWarnMessage
} from "./message";

export interface IHyperbuildResult {
  code?: string;
  messages: {
    unknown: IHyperbuildUnknownMessage[];
    debug: IHyperbuildDebugMessage[];
    info: IHyperbuildInfoMessage[];
  };
  warnings: IHyperbuildWarnMessage[];
}
