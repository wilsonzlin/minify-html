package in.wilsonl.minifyhtml;

/**
 * Basic exception class representing minification errors.
 */
public class SyntaxException extends RuntimeException {
  private SyntaxException(String message) {
    super(message);
  }
}
