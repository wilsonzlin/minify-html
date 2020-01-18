package in.wilsonl.hyperbuild;

import java.nio.ByteBuffer;

import static in.wilsonl.hyperbuild.NativeLibraryLoader.loadLibraryFromJar;
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
      throw new RuntimeException(format("Platform not supported (%s, %s)", osName, osArch));
    }

    try {
      loadLibraryFromJar(format("/%s-%s.nativelib", nativeLibNameOs, nativeLibNameArch));
    } catch (Exception e) {
      throw new RuntimeException("Failed to load native library", e);
    }
  }

  public static native int minifyInPlace(ByteBuffer code) throws HyperbuildException;

  public static native String minify(String code) throws HyperbuildException;
}
