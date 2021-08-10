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
  public final boolean keep_spaces_between_attributes;
  public final boolean minify_css;
  public final boolean minify_js;
  public final boolean remove_bangs;
  public final boolean remove_processing_instructions;

  public Configuration(
    boolean do_not_minify_doctype,
    boolean ensure_spec_compliant_unquoted_attribute_values,
    boolean keep_closing_tags,
    boolean keep_comments,
    boolean keep_html_and_head_opening_tags,
    boolean keep_spaces_between_attributes,
    boolean minify_css,
    boolean minify_js,
    boolean remove_bangs,
    boolean remove_processing_instructions
  ) {
    this.do_not_minify_doctype = do_not_minify_doctype;
    this.ensure_spec_compliant_unquoted_attribute_values = ensure_spec_compliant_unquoted_attribute_values;
    this.keep_closing_tags = keep_closing_tags;
    this.keep_comments = keep_comments;
    this.keep_html_and_head_opening_tags = keep_html_and_head_opening_tags;
    this.keep_spaces_between_attributes = keep_spaces_between_attributes;
    this.minify_css = minify_css;
    this.minify_js = minify_js;
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
    private boolean keep_spaces_between_attributes = false;
    private boolean minify_css = false;
    private boolean minify_js = false;
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
        this.keep_spaces_between_attributes,
        this.minify_css,
        this.minify_js,
        this.remove_bangs,
        this.remove_processing_instructions
      );
    }
  }
}
