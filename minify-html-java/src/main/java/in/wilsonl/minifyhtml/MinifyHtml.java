package in.wilsonl.minifyhtml;

import java.io.File;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

/**
 * Class containing only static methods and exception classes. Cannot be instantiated.
 * Methods call to native compiled Rust code using JNI.
 * When this class is loaded, a static initialiser will attempt to load a prebuilt native library for the running operating system and architecture from the JAR. If it cannot, a {@link RuntimeException} will be thrown.
 */
public class MinifyHtml {
  static {
    final String osName = System.getProperty("os.name").toLowerCase();
    final String osArch = System.getProperty("os.arch").toLowerCase();

    final String nativeLibNameOs = getNativeLibNameOs(osName);
    final String nativeLibNameArch = getNativeLibNameArch(osArch);

    if (nativeLibNameOs == null || nativeLibNameArch == null) {
      throw new RuntimeException(String.format("Platform not supported (os.name=%s, os.arch=%s)", osName, osArch));
    }

    final String nativeLibFile = String.format("/%s-%s.nativelib", nativeLibNameOs, nativeLibNameArch);

    try (InputStream is = MinifyHtml.class.getResourceAsStream(nativeLibFile)) {
      final File temp = File.createTempFile("minify-html-java-nativelib", nativeLibFile.substring(1));
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
   * Minify HTML code represented as a {@link String}.
   * The {@link String} will be copied to a UTF-8 byte array in native code, and then copied back into a Java {@link String}.
   *
   * @param code HTML code to minify
   * @param cfg  {@link Configuration} minification settings to use
   * @return minified HTML code
   */
  public static native String minify(String code, Configuration cfg);

  private static String getNativeLibNameOs(String osName) {
    if (osName.startsWith("windows")) {
      return "win";
    } else if (osName.startsWith("linux")) {
      return "linux";
    } else if (osName.startsWith("mac")) {
      return "mac";
    } else {
      return null;
    }
  }

  private static String getNativeLibNameArch(String osArch) {
    if (osArch.equals("amd64") || osArch.equals("x86_64")) {
      return "x64";
    } else if (osArch.equals("arm64") || osArch.equals("aarch64")) {
      return "aarch64";
    } else {
      return null;
    }
  }
}
