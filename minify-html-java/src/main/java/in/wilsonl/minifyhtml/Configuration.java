package in.wilsonl.minifyhtml;

// WARNING: Do not manually edit, use Configuration.java.gen.js.

/**
 * Class representing minification configuration.
 */
public class Configuration {
  public final boolean allow_noncompliant_unquoted_attribute_values;
  public final boolean allow_optimal_entities;
  public final boolean allow_removing_spaces_between_attributes;
  public final boolean keep_closing_tags;
  public final boolean keep_comments;
  public final boolean keep_html_and_head_opening_tags;
  public final boolean keep_input_type_text_attr;
  public final boolean keep_ssi_comments;
  public final boolean minify_css;
  public final boolean minify_doctype;
  public final boolean minify_js;
  public final boolean preserve_brace_template_syntax;
  public final boolean preserve_chevron_percent_template_syntax;
  public final boolean remove_bangs;
  public final boolean remove_processing_instructions;

  private Configuration(
    boolean allow_noncompliant_unquoted_attribute_values,
    boolean allow_optimal_entities,
    boolean allow_removing_spaces_between_attributes,
    boolean keep_closing_tags,
    boolean keep_comments,
    boolean keep_html_and_head_opening_tags,
    boolean keep_input_type_text_attr,
    boolean keep_ssi_comments,
    boolean minify_css,
    boolean minify_doctype,
    boolean minify_js,
    boolean preserve_brace_template_syntax,
    boolean preserve_chevron_percent_template_syntax,
    boolean remove_bangs,
    boolean remove_processing_instructions
  ) {
    this.allow_noncompliant_unquoted_attribute_values = allow_noncompliant_unquoted_attribute_values;
    this.allow_optimal_entities = allow_optimal_entities;
    this.allow_removing_spaces_between_attributes = allow_removing_spaces_between_attributes;
    this.keep_closing_tags = keep_closing_tags;
    this.keep_comments = keep_comments;
    this.keep_html_and_head_opening_tags = keep_html_and_head_opening_tags;
    this.keep_input_type_text_attr = keep_input_type_text_attr;
    this.keep_ssi_comments = keep_ssi_comments;
    this.minify_css = minify_css;
    this.minify_doctype = minify_doctype;
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
    private boolean allow_noncompliant_unquoted_attribute_values = false;
    private boolean allow_optimal_entities = false;
    private boolean allow_removing_spaces_between_attributes = false;
    private boolean keep_closing_tags = false;
    private boolean keep_comments = false;
    private boolean keep_html_and_head_opening_tags = false;
    private boolean keep_input_type_text_attr = false;
    private boolean keep_ssi_comments = false;
    private boolean minify_css = false;
    private boolean minify_doctype = false;
    private boolean minify_js = false;
    private boolean preserve_brace_template_syntax = false;
    private boolean preserve_chevron_percent_template_syntax = false;
    private boolean remove_bangs = false;
    private boolean remove_processing_instructions = false;

    public Builder setAllowNoncompliantUnquotedAttributeValues(boolean v) {
      this.allow_noncompliant_unquoted_attribute_values = v;
      return this;
    }
    public Builder setAllowOptimalEntities(boolean v) {
      this.allow_optimal_entities = v;
      return this;
    }
    public Builder setAllowRemovingSpacesBetweenAttributes(boolean v) {
      this.allow_removing_spaces_between_attributes = v;
      return this;
    }
    public Builder setKeepClosingTags(boolean v) {
      this.keep_closing_tags = v;
      return this;
    }
    public Builder setKeepComments(boolean v) {
      this.keep_comments = v;
      return this;
    }
    public Builder setKeepHtmlAndHeadOpeningTags(boolean v) {
      this.keep_html_and_head_opening_tags = v;
      return this;
    }
    public Builder setKeepInputTypeTextAttr(boolean v) {
      this.keep_input_type_text_attr = v;
      return this;
    }
    public Builder setKeepSsiComments(boolean v) {
      this.keep_ssi_comments = v;
      return this;
    }
    public Builder setMinifyCss(boolean v) {
      this.minify_css = v;
      return this;
    }
    public Builder setMinifyDoctype(boolean v) {
      this.minify_doctype = v;
      return this;
    }
    public Builder setMinifyJs(boolean v) {
      this.minify_js = v;
      return this;
    }
    public Builder setPreserveBraceTemplateSyntax(boolean v) {
      this.preserve_brace_template_syntax = v;
      return this;
    }
    public Builder setPreserveChevronPercentTemplateSyntax(boolean v) {
      this.preserve_chevron_percent_template_syntax = v;
      return this;
    }
    public Builder setRemoveBangs(boolean v) {
      this.remove_bangs = v;
      return this;
    }
    public Builder setRemoveProcessingInstructions(boolean v) {
      this.remove_processing_instructions = v;
      return this;
    }

    public Configuration build() {
      return new Configuration(
        this.allow_noncompliant_unquoted_attribute_values,
        this.allow_optimal_entities,
        this.allow_removing_spaces_between_attributes,
        this.keep_closing_tags,
        this.keep_comments,
        this.keep_html_and_head_opening_tags,
        this.keep_input_type_text_attr,
        this.keep_ssi_comments,
        this.minify_css,
        this.minify_doctype,
        this.minify_js,
        this.preserve_brace_template_syntax,
        this.preserve_chevron_percent_template_syntax,
        this.remove_bangs,
        this.remove_processing_instructions
      );
    }
  }
}
