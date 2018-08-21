declare var _em_init: any;
declare var _free: any;
declare var _hbe_err_code: any;
declare var _hbe_err_destroy: any;
declare var _hbe_err_message: any;
declare var _malloc: any;
declare var ALLOC_NORMAL: any;
declare var allocate: any;
declare var cwrap: any;
declare var getValue: any;
declare var lengthBytesUTF8: any;
declare var Pointer_stringify: any;
declare var stringToUTF8: any;
declare var UTF8ToString: any;

export interface IHyperbuildSettings<O extends string | undefined> {
  keep?: boolean;
  buffer?: boolean;

  inputCode?: string;
  inputFile?: string;
  outputFile?: O;

  suppress?: string[];

  MXcollapseWhitespace?: string[];
  MXdestroyWholeWhitespace?: string[];
  MXtrimWhitespace?: string[];

  trim_class_attr?: boolean;
  decode_entities?: boolean;
  min_conditional_comments?: boolean;
  remove_attr_quotes?: boolean;
  remove_comments?: boolean;
  remove_optional_tags?: boolean;
  remove_tag_whitespace?: boolean;
}

export class HyperbuildError extends Error {
  code: number;

  public constructor(code: number, message: string) {
    super();
    this.code = code;
    this.message = message;
  }
}

const hyperbuild_c_arg_types = [
  'pointer', // input_code
  'number', // input_code_length

  'string', // input_path

  'number', // output_code (pointer)
  'number', // output_code_length (pointer)

  'string', // output_path

  'boolean', // config_keep
  'boolean', // config_buffer

  'string', // suppress

  'boolean', // nondefault_ex_collapse_whitespace
  'string', // ex_collapse_whitespace
  'boolean', // nondefault_ex_destroy_whole_whitespace
  'string', // ex_destroy_whole_whitespace
  'boolean', // nondefault_ex_trim_whitespace
  'string', // ex_trim_whitespace

  'boolean', // trim_class_attr
  'boolean', // decode_entities
  'boolean', // min_conditional_comments
  'boolean', // remove_attr_quotes
  'boolean', // remove_comments
  'boolean', // remove_optional_tags
  'boolean', // remove_tag_whitespace
];

const hyperbuild_c = cwrap('em_entry', 'number', hyperbuild_c_arg_types);

export interface IHyperbuild {
  <O extends string | undefined>(settings: IHyperbuildSettings<O>): O
}

export function hyperbuild<O extends string | undefined>({
  keep = false,
  buffer = false,

  inputCode,
  inputFile,
  outputFile,

  suppress,

  MXcollapseWhitespace,
  MXdestroyWholeWhitespace,
  MXtrimWhitespace,

  trim_class_attr = true,
  decode_entities = true,
  min_conditional_comments = true,
  remove_attr_quotes = true,
  remove_comments = true,
  remove_optional_tags = true,
  remove_tag_whitespace = true,
}: IHyperbuildSettings<O>): O {
  let args: any[] = hyperbuild_c_arg_types.map(() => null);

  let input_code_ptr = 0;

  let output_ptr = 0;
  let output_size_ptr = 0;

  if (inputCode != undefined) {
    let bytes = lengthBytesUTF8(inputCode);
    args[0] = input_code_ptr = _malloc(bytes + 1);
    stringToUTF8(inputCode, input_code_ptr, bytes + 1);
    args[1] = bytes;
  } else {
    args[2] = inputFile;
  }

  if (outputFile == undefined) {
    // Pointers are 32-bit integers in Emscripten
    // https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.htm
    args[3] = output_ptr = allocate([0], "*", ALLOC_NORMAL);
    args[4] = output_size_ptr = allocate([0], "*", ALLOC_NORMAL);
  } else {
    args[5] = outputFile;
  }

  args[6] = keep;
  args[7] = buffer;

  if (suppress != undefined) {
    args[8] = suppress.join(",");
  }

  if (MXcollapseWhitespace != undefined) {
    args[9] = true;
    args[10] = MXcollapseWhitespace.join(",");
  } else {
    args[9] = false;
  }
  if (MXdestroyWholeWhitespace != undefined) {
    args[11] = true;
    args[12] = MXdestroyWholeWhitespace.join(",");
  } else {
    args[11] = false;
  }
  if (MXtrimWhitespace != undefined) {
    args[13] = true;
    args[14] = MXtrimWhitespace.join(",");
  } else {
    args[13] = false;
  }

  args[15] = trim_class_attr;
  args[16] = decode_entities;
  args[17] = min_conditional_comments;
  args[18] = remove_attr_quotes;
  args[19] = remove_comments;
  args[20] = remove_optional_tags;
  args[21] = remove_tag_whitespace;

  let err = null;

  let err_ptr = hyperbuild_c.apply(undefined, args);
  if (err_ptr) {
    let err_code = _hbe_err_code(err_ptr);
    // TODO Does this work with non-ASCII?
    // TODO Needs length as error message might contain NUL
    let err_message = Pointer_stringify(_hbe_err_message(err_ptr));
    _hbe_err_destroy(err_ptr);
    err = new HyperbuildError(err_code, err_message);
  }

  let rv: O = undefined as O;

  if (output_ptr) {
    // TODO Figure out size_t
    let length = getValue(output_size_ptr, "i64");
    // TODO Does this work with non-ASCII?
    rv = Pointer_stringify(getValue(output_ptr, "*"), length) as O;
  }

  [input_code_ptr, output_ptr, output_size_ptr].forEach(ptr => {
    if (ptr) {
      _free(ptr);
    }
  });

  if (err) {
    throw err;
  }

  return rv;
}

let loaded = false;
const onload_functions: ((hyperbuild: IHyperbuild) => any)[] = [];
const onload_promises: Function[] = [];

export function onload(callback: (hyperbuild: IHyperbuild) => any) {
  if (loaded) {
    callback(hyperbuild);
  } else {
    onload_functions.push(callback);
  }
}

export function load(): Promise<IHyperbuild> {
  if (loaded) {
    return Promise.resolve(hyperbuild);
  } else {
    return new Promise(resolve => {
      onload_promises.push(resolve);
    });
  }
}

Module['onRuntimeInitialized'] = () => {
  _em_init();
  loaded = true;
  onload_functions.forEach(fn => {
    fn(hyperbuild);
  });
  onload_promises.forEach(res => {
    res(hyperbuild);
  });
};
