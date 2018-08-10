import {ChildProcess} from "child_process";
import {
  feedHyperbuild,
  HyperbuildCode,
  HyperbuildProcessCallback,
  IHyperbuildSettings,
  launchHyperbuild
} from "../bin/hyperbuild";
import {IHyperbuildResult} from "../res/result";

interface IHyperbuildPoolQueueEntry {
  code: HyperbuildCode;
  end: HyperbuildProcessCallback;
}

export class HyperbuildPool {
  private readonly settings: IHyperbuildSettings;
  private readonly slots: (ChildProcess | null)[];
  private readonly callbacks: (HyperbuildProcessCallback | null)[];
  private readonly queue: IHyperbuildPoolQueueEntry[];

  private constructor (instancesCount: number, settings: IHyperbuildSettings) {
    if (settings.in != undefined || settings.out != undefined) {
      throw new TypeError(`Cannot provide --in and --out options to pool`);
    }

    this.settings = settings;
    this.slots = Array(instancesCount).fill(null);
    this.callbacks = Array(instancesCount).fill(null);
    this.queue = [];

    this.slots.forEach((_, s) => this._createInstance(s));
  }

  private _createInstance (slot: number) {
    this.slots[slot] = launchHyperbuild(this.settings, (err, res) => {
      let cb = this.callbacks[slot];

      if (!cb) {
        throw new ReferenceError(`No callback provided to handle response to instance in slot ${slot}`);
      }

      this.callbacks[slot] = null;

      cb(err, res);

      this.slots[slot] = null;
      this._createInstance(slot);
    });

    this._processQueue();
  }

  private _processQueue (): void {
    while (this.queue.length) {
      let free_slot = this.slots.findIndex(s => s != null);
      if (free_slot == -1) {
        return;
      }

      let entry = this.queue.shift()!;
      let instance = this.slots[free_slot]!;

      this.slots[free_slot] = null;
      this.callbacks[free_slot] = entry.end;
      feedHyperbuild(instance, entry.code);
    }
  }

  public run (code: HyperbuildCode): Promise<IHyperbuildResult> {
    return new Promise((resolve, reject) => {
      this.queue.push({
        code: code,
        end: (err, res) => {
          if (err) {
            reject(err);
          } else {
            resolve(res);
          }
        },
      });
      this._processQueue();
    });
  }
}
