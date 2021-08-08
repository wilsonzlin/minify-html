# Benchmark runners

- Each directory should have an executable `./build` and `./run`.
- The runners should use the following environment variables:
  - `MHB_ITERATIONS`: times to run each input.
  - `MHB_INPUT_DIR`: path to directory containing inputs. Files should be read from this directory and used as the inputs.
  - `MHB_HTML_ONLY`: if set to `1`, `minify_css` and `minify_js` should be disabled.
- The output should be a JSON array of tuples, where each tuples contains the input name, output size, iterations, and execution time in seconds (as a floating point value).
  - The execution time should be measured using high-precision monotonic system clocks where possible.
