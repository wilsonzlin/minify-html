package in.wilsonl.hyperbuild;

import java.util.Arrays;

import static in.wilsonl.hyperbuild.NativeLibraryLoader.loadLibraryFromJar;
import static java.lang.String.format;
import static java.nio.charset.StandardCharsets.UTF_8;

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
      throw new RuntimeException(format("Platform not supported (%s, %s)", osName, osArch));
    }

    try {
      loadLibraryFromJar(format("%s-%s.nativelib", nativeLibNameOs, nativeLibNameArch));
    } catch (Exception e) {
      throw new RuntimeException("Failed to load native library", e);
    }
  }

  public static native int minifyInPlace(byte[] code) throws HyperbuildException;

  public static byte[] minify(byte[] code) throws HyperbuildException {
    int size = minifyInPlace(code);
    return Arrays.copyOf(code, size);
  }

  public static String minify(String code) throws HyperbuildException {
    return new String(Hyperbuild.minify(code.getBytes(UTF_8)), UTF_8);
  }
}
