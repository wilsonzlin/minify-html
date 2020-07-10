package in.wilsonl.hyperbuild;

import java.io.File;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

import static java.lang.String.format;

/**
 * Class containing only static methods and exception classes. Cannot be instantiated.
 * Methods call to native compiled Rust code using JNI.
 * When this class is loaded, a static initialiser will attempt to load a prebuilt native library for the running operating system and architecture from the JAR. If it cannot, a {@link RuntimeException} will be thrown.
 */
public class Hyperbuild {
  static {
    String osName = System.getProperty("os.name").toLowerCase();
    String osArch = System.getProperty("os.arch").toLowerCase();

    String nativeLibNameOs = osName.startsWith("windows")
      ? "windows"
      : osName.startsWith("linux")
        ? "linux"
        : osName.startsWith("mac")
          ? "macos"
          : null;
    String nativeLibNameArch = osArch.equals("amd64") ? "x86_64" : null;

    if (nativeLibNameOs == null || nativeLibNameArch == null) {
      throw new RuntimeException(format("Platform not supported (os.name=%s, os.arch=%s)", osName, osArch));
    }

    String nativeLibFile = format("/%s-%s.nativelib", nativeLibNameOs, nativeLibNameArch);

    try (InputStream is = Hyperbuild.class.getResourceAsStream(nativeLibFile)) {
      File temp = File.createTempFile("hyperbuild-java-nativelib", nativeLibFile.substring(1));
      temp.deleteOnExit();
      Files.copy(is, temp.toPath(), StandardCopyOption.REPLACE_EXISTING);
      System.load(temp.getAbsolutePath());
    } catch (Exception e) {
      throw new RuntimeException("Failed to load native library", e);
    }
  }

  private Hyperbuild() {
  }

  /**
   * Minify UTF-8 HTML code contents of a {@link ByteBuffer} instance in place.
   * The backing data will be mutated. Returns the length of the minified portion of the ByteBuffer.
   * The ByteBuffer must be direct, otherwise {@link IllegalArgumentException} will be thrown.
   * If the code fails to be minified, a {@link SyntaxException} will be thrown with a descriptive English message and position in code where the error occurred.
   *
   * @param code {@link ByteBuffer} containing HTML code to minify
   * @param cfg {@link Configuration} minification settings to use
   * @return length of the written minified code in the {@link ByteBuffer}
   */
  public static native int minifyInPlace(ByteBuffer code, Configuration cfg);

  /**
   * Minify HTML code represented as a {@link String}.
   * The {@link String} will be copied to a UTF-8 byte array in native code, and then copied back into a Java {@link String}.
   * If the code fails to be minified, a {@link SyntaxException} will be thrown with a descriptive English message and position in code where the error occurred.
   *
   * @param code HTML code to minify
   * @param cfg {@link Configuration} minification settings to use
   * @return minified HTML code
   */
  public static native String minify(String code, Configuration cfg);

  /**
   * Basic exception class representing minification errors.
   */
  public static class SyntaxException extends RuntimeException {
    private SyntaxException(String message) {
      super(message);
    }
  }

  /**
   * Class representing minification configuration.
   */
  public static class Configuration {
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
}
