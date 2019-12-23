# Processing

## Redundant requires

Sometimes the code will look like it duplicates matching logic. For example:

```rust
fn process_comment(proc: &mut Proc) -> () {
	proc.matches("<!--").require_reason("comment").skip();

	proc.while_not_matches("-->").skip();

	proc.matches("-->").require_reason("comment end").skip();
}
```

At first glance, it might appear that the second call `while_not_matches` makes it redundant to require it again immediately afterwards. However, it's possible that the `while_not_matches` actually stops for some other reason, such as reaching EOF. Even if it's guaranteed, it's still nice to have a declared invariant, like an assertion statement.
