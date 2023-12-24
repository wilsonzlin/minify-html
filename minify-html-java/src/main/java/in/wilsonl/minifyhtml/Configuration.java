package in.wilsonl.minifyhtml;

/**
 * Class representing minification configuration.
 */
public class Configuration {
  public final boolean do_not_minify_doctype;
  public final boolean ensure_spec_compliant_unquoted_attribute_values;
  public final boolean keep_closing_tags;
  public final boolean keep_comments;
  public final boolean keep_html_and_head_opening_tags;
  public final boolean keep_input_type_text_attr;
  public final boolean keep_spaces_between_attributes;
  public final boolean keep_ssi_comments;
  public final boolean minify_css;
  public final boolean minify_js;
  public final boolean preserve_brace_template_syntax;
  public final boolean preserve_chevron_percent_template_syntax;
  public final boolean remove_bangs;
  public final boolean remove_processing_instructions;

  private Configuration(
    boolean do_not_minify_doctype,
    boolean ensure_spec_compliant_unquoted_attribute_values,
    boolean keep_closing_tags,
    boolean keep_comments,
    boolean keep_html_and_head_opening_tags,
    boolean keep_input_type_text_attr,
    boolean keep_spaces_between_attributes,
    boolean keep_ssi_comments,
    boolean minify_css,
    boolean minify_js,
    boolean preserve_brace_template_syntax,
    boolean preserve_chevron_percent_template_syntax,
    boolean remove_bangs,
    boolean remove_processing_instructions
  ) {
    this.do_not_minify_doctype = do_not_minify_doctype;
    this.ensure_spec_compliant_unquoted_attribute_values = ensure_spec_compliant_unquoted_attribute_values;
    this.keep_closing_tags = keep_closing_tags;
    this.keep_comments = keep_comments;
    this.keep_html_and_head_opening_tags = keep_html_and_head_opening_tags;
    this.keep_input_type_text_attr = keep_input_type_text_attr;
    this.keep_spaces_between_attributes = keep_spaces_between_attributes;
    this.keep_ssi_comments = keep_ssi_comments;
    this.minify_css = minify_css;
    this.minify_js = minify_js;
    this.preserve_brace_template_syntax = preserve_brace_template_syntax;
    this.preserve_chevron_percent_template_syntax = preserve_chevron_percent_template_syntax;
    this.remove_bangs = remove_bangs;
    this.remove_processing_instructions = remove_processing_instructions;
  }

  /**
   * Builder to help create configuration.
   */
  public static class Builder {
    private boolean do_not_minify_doctype = false;
    private boolean ensure_spec_compliant_unquoted_attribute_values = false;
    private boolean keep_closing_tags = false;
    private boolean keep_comments = false;
    private boolean keep_html_and_head_opening_tags = false;
    private boolean keep_input_type_text_attr = false;
    private boolean keep_spaces_between_attributes = false;
    private boolean keep_ssi_comments = false;
    private boolean minify_css = false;
    private boolean minify_js = false;
    private boolean preserve_brace_template_syntax = false;
    private boolean preserve_chevron_percent_template_syntax = false;
    private boolean remove_bangs = false;
    private boolean remove_processing_instructions = false;

    public Builder setDoNotMinifyDoctype(boolean val) {
      this.do_not_minify_doctype = val;
      return this;
    }

    public Builder setEnsureSpecCompliantUnquotedAttributeValues(boolean val) {
      this.ensure_spec_compliant_unquoted_attribute_values = val;
      return this;
    }

    public Builder setKeepClosingTags(boolean val) {
      this.keep_closing_tags = val;
      return this;
    }

    public Builder setKeepComments(boolean val) {
      this.keep_comments = val;
      return this;
    }

    public Builder setKeepInputTypeTextAttr(boolean val) {
      this.keep_input_type_text_attr = val;
      return this;
    }

    public Builder setKeepSsiComments(boolean val) {
      this.keep_ssi_comments = val;
      return this;
    }

    public Builder setKeepHtmlAndHeadOpeningTags(boolean val) {
      this.keep_html_and_head_opening_tags = val;
      return this;
    }

    public Builder setKeepSpacesBetweenAttributes(boolean val) {
      this.keep_spaces_between_attributes = val;
      return this;
    }

    public Builder setMinifyCss(boolean val) {
      this.minify_css = val;
      return this;
    }

    public Builder setMinifyJs(boolean val) {
      this.minify_js = val;
      return this;
    }

    public Builder setRemoveBangs(boolean val) {
      this.remove_bangs = val;
      return this;
    }

    public Builder setPreserveBraceTemplateSyntax(boolean val) {
      this.preserve_brace_template_syntax = val;
      return this;
    }

    public Builder setPreserveChevronPercentTemplateSyntax(boolean val) {
      this.preserve_chevron_percent_template_syntax = val;
      return this;
    }

    public Builder setRemoveProcessingInstructions(boolean val) {
      this.remove_processing_instructions = val;
      return this;
    }

    public Configuration build() {
      return new Configuration(
        this.do_not_minify_doctype,
        this.ensure_spec_compliant_unquoted_attribute_values,
        this.keep_closing_tags,
        this.keep_comments,
        this.keep_html_and_head_opening_tags,
        this.keep_input_type_text_attr,
        this.keep_spaces_between_attributes,
        this.keep_ssi_comments,
        this.minify_css,
        this.minify_js,
        this.preserve_brace_template_syntax,
        this.preserve_chevron_percent_template_syntax,
        this.remove_bangs,
        this.remove_processing_instructions
      );
    }
  }
}
