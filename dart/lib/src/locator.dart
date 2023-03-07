import 'dart:ffi';
import 'dart:io';

/// Attempts to locate the MinifyHtml dynamic library.
///
/// Throws [MinifyHtmlLocatorError] if the dynamic library could not be found.
DynamicLibrary loadDynamicLibrary() {
  String locate(String libName) {
    final path = _packageRootUri(Platform.script.resolve('./'), libName) ??
        _packageRootUri(Directory.current.uri, libName);

    if (path != null) {
      return path;
    }

    final toolLib = Directory.current.uri
        .resolve("$_minifyhtmlToolDir$libName")
        .toFilePath();

    if (FileSystemEntity.isFileSync(toolLib)) {
      return toolLib;
    }

    throw MinifyHtmlLocatorError('MinifyHtml library not found');
  }

  if (Platform.isIOS) {
    return DynamicLibrary.process();
  } else if (Platform.isMacOS) {
    return DynamicLibrary.open(locate(appleLib));
  } else if (Platform.isWindows) {
    return DynamicLibrary.open(locate(windowsLib));
  } else if (Platform.isLinux) {
    return DynamicLibrary.open(locate(linuxLib));
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

String? _packageRootUri(Uri root, String libName) {
  do {
    final filePath = root.resolve(libName).toFilePath();
    print(filePath);
    if (FileSystemEntity.isFileSync(filePath)) {
      return filePath;
    }
  } while (root != (root = root.resolve('..')));
  return null;
}
