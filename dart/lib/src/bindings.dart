// ignore_for_file: non_constant_identifier_names, camel_case_types

import 'dart:ffi';

import 'package:ffi/ffi.dart';
import 'package:minify_html/src/locator.dart';

typedef minify_html_native = Int32 Function(
  Pointer<Utf8> input,
  Uint32 length,
  Bool do_not_minify_doctype,
  Bool ensure_spec_compliant_unquoted_attribute_values,
  Bool keep_closing_tags,
  Bool keep_html_and_head_opening_tags,
  Bool keep_spaces_between_attributes,
  Bool keep_comments,
  Bool minify_css,
  Bool minify_css_level_1,
  Bool minify_css_level_2,
  Bool minify_css_level_3,
  Bool minify_js,
  Bool remove_bangs,
  Bool remove_processing_instructions,
);

typedef get_last_result_native = Pointer<Utf8> Function();

typedef clear_last_result_native = Void Function();

class MinifyHtmlBindings {
  late DynamicLibrary _library;

  late int Function(
    Pointer<Utf8> input,
    int length,
    bool do_not_minify_doctype,
    bool ensure_spec_compliant_unquoted_attribute_values,
    bool keep_closing_tags,
    bool keep_html_and_head_opening_tags,
    bool keep_spaces_between_attributes,
    bool keep_comments,
    bool minify_css,
    bool minify_css_level_1,
    bool minify_css_level_2,
    bool minify_css_level_3,
    bool minify_js,
    bool remove_bangs,
    bool remove_processing_instructions,
  ) minifyHtml;

  late Pointer<Utf8> Function() getLastResult;

  late void Function() clearLastResult;

  MinifyHtmlBindings() {
    _library = loadDynamicLibrary();

    minifyHtml = _library
        .lookup<NativeFunction<minify_html_native>>("minify_html")
        .asFunction();
    getLastResult = _library
        .lookup<NativeFunction<get_last_result_native>>("get_last_result")
        .asFunction();
    clearLastResult = _library
        .lookup<NativeFunction<clear_last_result_native>>("clear_last_result")
        .asFunction();
  }
}

MinifyHtmlBindings? _cachedBindings;
MinifyHtmlBindings get bindings => _cachedBindings ??= MinifyHtmlBindings();
