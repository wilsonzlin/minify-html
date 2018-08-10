import {feedHyperbuild, HyperbuildCode, IHyperbuildSettings, launchHyperbuild} from "./bin/hyperbuild";
import {IHyperbuildResult} from "./res/result";

export * from "./pool/pool";
export * from "./res/error";
export * from "./res/message";
export * from "./res/result";

export interface IHyperbuildSingleUseSettings extends IHyperbuildSettings {
  code?: HyperbuildCode;
}

export function hyperbuild (settings: IHyperbuildSingleUseSettings): Promise<IHyperbuildResult> {
  let {code, ...cliSettings} = settings;

  return new Promise((resolve, reject) => {
    let proc = launchHyperbuild(cliSettings, (err, res) => {
      if (err) {
        reject(err);
      } else {
        resolve(res);
      }
    });

    if (code != undefined) {
      feedHyperbuild(proc, code);
    }
  });
}
