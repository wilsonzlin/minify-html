import 'package:ffi/ffi.dart';
import 'package:minify_html/src/bindings.dart';
import 'package:minify_html/src/models.dart';
import 'package:minify_html/src/resource.dart';

String minifyHtml(String source, [Cfg? cfg]) {
  cfg = cfg ??= Cfg();

  final srcC = Utf8Resource(source);

  try {
    final length = bindings.minifyHtml(
      srcC.unsafe(),
      srcC.length,
      cfg.doNotMinifyDoctype,
      cfg.ensureSpecCompliantUnquotedAttributeValues,
      cfg.keepClosingTags,
      cfg.keepHtmlAndHeadOpeningTags,
      cfg.keepSpacesBetweenAttributes,
      cfg.keepComments,
      cfg.minifyCss,
      cfg.minifyCssLevel1,
      cfg.minifyCssLevel2,
      cfg.minifyCssLevel3,
      cfg.minifyJs,
      cfg.removeBangs,
      cfg.removeProcessingInstructions,
    );

    return "";
  } finally {
    srcC.free();
  }
}
