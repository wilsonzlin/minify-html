package in.wilsonl.minifyhtml;

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
public class MinifyHtml {
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
    String nativeLibNameArch = osArch.equals("amd64") || osArch.equals("x86_64") ? "x86_64" : null;

    if (nativeLibNameOs == null || nativeLibNameArch == null) {
      throw new RuntimeException(format("Platform not supported (os.name=%s, os.arch=%s)", osName, osArch));
    }

    String nativeLibFile = format("/%s-%s.nativelib", nativeLibNameOs, nativeLibNameArch);

    try (InputStream is = MinifyHtml.class.getResourceAsStream(nativeLibFile)) {
      File temp = File.createTempFile("minify-html-java-nativelib", nativeLibFile.substring(1));
      temp.deleteOnExit();
      Files.copy(is, temp.toPath(), StandardCopyOption.REPLACE_EXISTING);
      System.load(temp.getAbsolutePath());
    } catch (Exception e) {
      throw new RuntimeException("Failed to load native library", e);
    }
  }

  private MinifyHtml() {
  }

  /**
   * Minify UTF-8 HTML code contents of a {@link ByteBuffer} instance in place.
   * The backing data will be mutated. Returns the length of the minified portion of the ByteBuffer.
   * The ByteBuffer must be direct, otherwise {@link IllegalArgumentException} will be thrown.
   * If the code fails to be minified, a {@link SyntaxException} will be thrown with a descriptive English message and position in code where the error occurred.
   *
   * @param code {@link ByteBuffer} containing HTML code to minify
   * @param cfg  {@link Configuration} minification settings to use
   * @return length of the written minified code in the {@link ByteBuffer}
   */
  public static native int minifyInPlace(ByteBuffer code, Configuration cfg);

  /**
   * Minify HTML code represented as a {@link String}.
   * The {@link String} will be copied to a UTF-8 byte array in native code, and then copied back into a Java {@link String}.
   * If the code fails to be minified, a {@link SyntaxException} will be thrown with a descriptive English message and position in code where the error occurred.
   *
   * @param code HTML code to minify
   * @param cfg  {@link Configuration} minification settings to use
   * @return minified HTML code
   */
  public static native String minify(String code, Configuration cfg);
}
