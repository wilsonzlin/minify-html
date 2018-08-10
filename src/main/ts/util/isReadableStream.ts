import ReadableStream = NodeJS.ReadableStream;

export function isReadableStream (thing: any): thing is ReadableStream {
  return !!thing && typeof thing.pipe == "function";
}
