export class HyperbuildError extends Error {
  code: number;

  constructor (status: number, message: string) {
    super();
    this.code = status;
    this.message = `hyperbuild exited with status ${status} due to the following error:\n${message}`;
  }
}
