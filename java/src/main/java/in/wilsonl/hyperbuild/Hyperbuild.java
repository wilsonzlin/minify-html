package in.wilsonl.hyperbuild;

import java.util.Arrays;

import static java.nio.charset.StandardCharsets.UTF_8;

public class Hyperbuild {
  static {
    String nativeLibPath = Hyperbuild.class.getResource("libhyperbuild_java.so").getPath();
    System.load(nativeLibPath);
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
