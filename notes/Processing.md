# Processing

## Redundant requires

Sometimes the code will look like it does redundant matching logic. For example:

```rust
pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_seq(b"<!--").expect().discard());

    chain!(proc.match_while_not_seq(b"-->").discard());

    chain!(proc.match_seq(b"-->").require()?.discard());

    Ok(())
}
```

At first glance, it might appear that the second call `match_while_not_seq` makes it redundant to require it again immediately afterwards. However, it's possible that the `match_while_not_seq` actually stops for some other reason, such as reaching EOF. Even if it's guaranteed, it's still nice to have a declared invariant, like an assertion statement.
