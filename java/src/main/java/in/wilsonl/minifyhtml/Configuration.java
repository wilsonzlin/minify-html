package in.wilsonl.minifyhtml;

/**
 * Class representing minification configuration.
 */
public class Configuration {
  private final boolean minifyJs;

  public Configuration(boolean minifyJs) {
    this.minifyJs = minifyJs;
  }

  /**
   * Builder to help create configuration.
   */
  public static class Builder {
    private boolean minifyJs = false;

    public Builder setMinifyJs(boolean minifyJs) {
      this.minifyJs = minifyJs;
      return this;
    }

    public Configuration build() {
      return new Configuration(this.minifyJs);
    }
  }
}
