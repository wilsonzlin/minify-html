import 'dart:convert';
import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

final DynamicLibrary stdlib = DynamicLibrary.process();
final posixFree = stdlib.lookup<NativeFunction<Void Function(Pointer)>>("free");

class Utf8Resource implements Finalizable {
  static final NativeFinalizer _finalizer = NativeFinalizer(posixFree);

  /// [_cString] must never escape [Utf8Resource], otherwise the
  /// [_finalizer] will run prematurely.
  late final Pointer<Utf8> _cString;
  late final int length;

  Utf8Resource(String source) {
    final units = utf8.encode(source);
    length = units.length;

    final Pointer<Uint8> result = malloc<Uint8>(units.length + 1);
    final Uint8List nativeString = result.asTypedList(units.length + 1);
    nativeString.setAll(0, units);
    _cString = result.cast();

    _finalizer.attach(this, _cString.cast(), detach: this);
  }

  void free() {
    _finalizer.detach(this);
    calloc.free(_cString);
  }

  /// Ensure this [Utf8Resource] stays in scope longer than the inner resource.
  Pointer<Utf8> unsafe() => _cString;
}
