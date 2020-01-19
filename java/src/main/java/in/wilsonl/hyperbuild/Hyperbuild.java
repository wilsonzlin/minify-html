package in.wilsonl.hyperbuild;

import java.io.File;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

import static java.lang.String.format;

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

  public static native int minifyInPlace(ByteBuffer code) throws SyntaxException;

  public static native String minify(String code) throws SyntaxException;

  public static class SyntaxException extends RuntimeException {
    private SyntaxException(String message) {
      super(message);
    }
  }
}
