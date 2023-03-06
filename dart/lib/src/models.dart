class Cfg {
  const Cfg({
    this.doNotMinifyDoctype = false,
    this.ensureSpecCompliantUnquotedAttributeValues = false,
    this.keepClosingTags = false,
    this.keepHtmlAndHeadOpeningTags = false,
    this.keepSpacesBetweenAttributes = false,
    this.keepComments = false,
    this.minifyCss = false,
    this.minifyCssLevel1 = false,
    this.minifyCssLevel2 = false,
    this.minifyCssLevel3 = false,
    this.minifyJs = false,
    this.removeBangs = false,
    this.removeProcessingInstructions = false,
  });

  const Cfg.specCompliant()
      : this(
          doNotMinifyDoctype: true,
          ensureSpecCompliantUnquotedAttributeValues: true,
          keepSpacesBetweenAttributes: true,
        );

  final bool doNotMinifyDoctype;
  final bool ensureSpecCompliantUnquotedAttributeValues;
  final bool keepClosingTags;
  final bool keepHtmlAndHeadOpeningTags;
  final bool keepSpacesBetweenAttributes;
  final bool keepComments;
  final bool minifyCss;
  final bool minifyCssLevel1;
  final bool minifyCssLevel2;
  final bool minifyCssLevel3;
  final bool minifyJs;
  final bool removeBangs;
  final bool removeProcessingInstructions;
}
