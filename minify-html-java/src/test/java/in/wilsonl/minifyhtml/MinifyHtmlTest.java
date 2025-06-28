package in.wilsonl.minifyhtml;

import java.lang.reflect.Method;
import lombok.val;
import org.junit.jupiter.api.Test;
import org.junit.platform.commons.util.ReflectionUtils;

import static org.junit.jupiter.api.Assertions.*;

class MinifyHtmlTest {

  static final Configuration DEFAULT_CONFIG = new Configuration.Builder().build();

  @Test
  void testMinifyHtmlCallsNativeFunction() {
    final String input = "<html>   <body>   Hello   World!   </body>   </html>";
    final String expected = "<body>Hello World!";
    assertEquals(expected, MinifyHtml.minify(input, DEFAULT_CONFIG));
    assertEquals("", MinifyHtml.minify("", DEFAULT_CONFIG));
  }

  @Test
  void testNullParametersHandled() {
    assertThrows(IllegalArgumentException.class, () -> MinifyHtml.minify(null, DEFAULT_CONFIG));
    assertThrows(IllegalArgumentException.class, () -> MinifyHtml.minify("<html></html>", null));
    assertThrows(IllegalArgumentException.class, () -> MinifyHtml.minify(null, null));
  }

  @Test
  void testAllConfigsEnabled() {
    // dynamically find all config flags in Configuration.Builder
    val configSetters =
        ReflectionUtils.findMethods(Configuration.Builder.class, this::isBooleanSetter);
    val configBuilder = new Configuration.Builder();
    // enable all boolean configuration options
    assertDoesNotThrow(
        () -> {
          for (val method : configSetters) {
            method.invoke(configBuilder, true);
          }
        });

    val config = configBuilder.build();

    val configGetters = ReflectionUtils.findMethods(Configuration.class, this::isBooleanGetter);
    // verify all boolean configuration options are enabled
    assertDoesNotThrow(
        () -> {
          for (val method : configGetters) {
            assertTrue(
                (boolean) method.invoke(config),
                "Configuration option " + method.getName() + " should be true");
          }
        });

    final String input = "<html>   <body>  Hello   World!   </body>   </html>";
    final String expected = "<html><body>Hello World!</body></html>";
    assertEquals(expected, MinifyHtml.minify(input, config));
  }

  private boolean isBooleanSetter(Method method) {
    return method.getName().startsWith("set")
        && method.getParameterCount() == 1
        && method.getParameterTypes()[0] == boolean.class;
  }

  private boolean isBooleanGetter(Method method) {
    return method.getName().startsWith("is")
        && method.getParameterCount() == 0
        && method.getReturnType() == boolean.class;
  }
}
