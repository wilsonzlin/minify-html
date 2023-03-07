import 'dart:ffi';
import 'dart:io';

/// Attempts to locate the MinifyHtml dynamic library.
///
/// Throws [MinifyHtmlLocatorError] if the dynamic library could not be found.
DynamicLibrary loadDynamicLibrary() {
  String locate(String libName) {
    final dir = '$_minifyhtmlToolDir$libName';
    final value = _packageRootUri(Platform.script.resolve('./')) ??
        _packageRootUri(Directory.current.uri);
    if (value != null) {
      return value.resolve(dir).toFilePath();
    } else {
      throw MinifyHtmlLocatorError(
        'MinifyHtml library not found. Did you run `$invocationString`?',
      );
    }
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

const _pkgConfigFile = '.dart_tool/package_config.json';

Uri? _packageRootUri(Uri root) {
  do {
    print(root);
    if (FileSystemEntity.isFileSync(
      root.resolve(_pkgConfigFile).toFilePath(),
    )) {
      return root;
    }
  } while (root != (root = root.resolve('..')));
  return null;
}
