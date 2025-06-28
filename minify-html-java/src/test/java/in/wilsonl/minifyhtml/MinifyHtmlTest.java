package in.wilsonl.minifyhtml;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class MinifyHtmlTest {

  static final Configuration DEFAULT_CONFIG = new Configuration.Builder().build();

  @Test
  void testMinifyHtmlCallsNativeFunction() {
    final String input = "<html>   <body>   Hello   World!   </body>   </html>";
    final String expected = "<body>Hello World!";
    assertEquals(expected, MinifyHtml.minify(input, DEFAULT_CONFIG), "Basic HTML minification failed");
    assertEquals("", MinifyHtml.minify("", DEFAULT_CONFIG));
  }

  @Test
  void testNullParametersHandled() {
    assertThrows(IllegalArgumentException.class, () -> MinifyHtml.minify(null, DEFAULT_CONFIG));
    assertThrows(IllegalArgumentException.class, () -> MinifyHtml.minify("<html></html>", null));
    assertThrows(IllegalArgumentException.class, () -> MinifyHtml.minify(null, null));
  }
}
