import 'dart:ffi';
import 'dart:io';

/// Attempts to locate the MinifyHtml dynamic library.
///
/// Throws [MinifyHtmlLocatorError] if the dynamic library could not be found.
DynamicLibrary loadDynamicLibrary() {
  if (Platform.isIOS) {
    return DynamicLibrary.process();
  } else if (Platform.isMacOS) {
    return _locateOrError(appleLib);
  } else if (Platform.isWindows) {
    return _locate(windowsLib) ?? DynamicLibrary.executable();
  } else if (Platform.isLinux) {
    return _locateOrError(linuxLib);
  } else if (Platform.isAndroid) {
    return DynamicLibrary.open(linuxLib);
  } else if (Platform.isFuchsia) {
    throw MinifyHtmlLocatorError(
      'MinifyHtml is currently not supported on Fuchsia.',
    );
  } else {
    throw MinifyHtmlLocatorError(
      'MinifyHtml is currently not supported on this platform.',
    );
  }
}

/// This error is thrown when the dynamic library could not be found.
class MinifyHtmlLocatorError extends Error {
  final String message;

  MinifyHtmlLocatorError(
    this.message,
  );

  @override
  String toString() => 'MinifyHtmlLocatorError: $message';
}

/// The command that can be used to set up this package.
const invocationString = 'dart run minifyhtml:setup';

/// The expected name of the MinifyHtml library when compiled for Apple devices.
const appleLib = 'libminifyhtml.dylib';

/// The expected name of the MinifyHtml library when compiled for Linux devices.
const linuxLib = 'libminifyhtml.so';

/// The expected name of the MinifyHtml library when compiled for Windows devices.
const windowsLib = 'minifyhtml.dll';

const _minifyhtmlToolDir = '.dart_tool/minifyhtml/';

DynamicLibrary? _locate(String libName) {
  if (FileSystemEntity.isFileSync(libName)) {
    return DynamicLibrary.open(libName);
  }

  final toolLib =
      Directory.current.uri.resolve("$_minifyhtmlToolDir$libName").toFilePath();
  if (FileSystemEntity.isFileSync(toolLib)) {
    return DynamicLibrary.open(toolLib);
  }

  return null;
}

DynamicLibrary _locateOrError(String libName) {
  final value = _locate(libName);
  if (value != null) {
    return value;
  } else {
    throw MinifyHtmlLocatorError('MinifyHtml library not found');
  }
}
