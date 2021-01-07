package in.wilsonl.minifyhtml;

/**
 * Class representing minification configuration.
 */
public class Configuration {
  private final boolean minifyJs;
  private final boolean minifyCss;

  public Configuration(boolean minifyJs, boolean minifyCss) {
    this.minifyJs = minifyJs;
    this.minifyCss = minifyCss;
  }

  /**
   * Builder to help create configuration.
   */
  public static class Builder {
    private boolean minifyJs = false;
    private boolean minifyCss = false;

    public Builder setMinifyJs(boolean minifyJs) {
      this.minifyJs = minifyJs;
      return this;
    }

    public Builder setMinifyCss(boolean minifyCss) {
      this.minifyCss = minifyCss;
      return this;
    }

    public Configuration build() {
      return new Configuration(this.minifyJs, this.minifyCss);
    }
  }
}
