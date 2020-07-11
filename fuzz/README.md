# Fuzzing

This folder contains a Cargo package for a fuzz target that can be used for fuzzing with [american fuzzy lop](https://github.com/google/AFL). Fuzzing has found many rare bugs and unhandled edge cases that cause crashes and is invaluable for improving the reliability of minify-html.

## Inputs

Initial inputs can be found in the [in](./in) folder.

## Building

```bash
cargo afl build
```

## Running

```bash
cargo afl fuzz -i in -o out target/debug/minify-html-fuzz-target
```

## Results

Inputs that cause a crash are found in the `out/crashes` folder.
