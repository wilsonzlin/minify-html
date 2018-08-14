# Error handling

## Error structs

Errors are represented using `hbe_err_s` structs (type `hbe_err_t`). It has two fields:

- `code`: A value from the enum `hbe_errcode` (type `hbe_errcode_t`).
- `message`: A character array (`hb_char_t *`) describing the error and providing context.

## Error-prone functions

Every function that may result in errors should declare `hbe_err_t *hbe_err` as its first parameter.

Functions can result in errors if:

- it calls any function that may result in an error
- it sets the variable pointed to by `hbe_err`

If the function needs to do cleanup operations, it should declare a `finally:` label at the end of the function and put the cleanup code there. If the function returns a value, the function should start with a `rv_t rv = 0;` declaration (where `rv_t` is the return type), and the `finally` section should end with a `return rv;`.

`rv` should be initialised because technically an error can occur at any time after it, including immediately afterwards.

## Creating errors

To create an error, use the `hbe_err_t hbe_error(hbe_errcode_t code, hb_char_t *message)` function.
The result should be set to `*hbe_err`, and then the function should return.

When an error occurs, the function should return some arbitrary return value such as `0`.
Return values from a function call are not considered reliable if errors occurred during their execution.

```c
int error_prone(hbe_err_t *hbe_err, char *msg) {
  if (some_error_condition) {
    *hbe_err = hbe_error(1, "Bad!");
    return 0;
  }

  printf("%s\n", msg);

  return 42;
}
```

To simplify this code, a macro is available:

```c
int error_prone(hbe_err_t *hbe_err, char *msg) {
  if (some_error_condition) {
    HBE_THROW(1, "Bad!");
    /* Translates to:
    *hbe_err = hbe_error(1, "Bad!");
    return 0;
    */
  }

  printf("%s\n", msg);

  return 42;
}
```

If the return type is `void`, use `HBE_THROW_V` instead of `HBE_THROW`.
If there is a cleanup section, use `HBE_THROW_F`.

## Handling errors

When a function call may result in an error, pass `hbe_err` to the function and check if the value dereferenced is not `NULL`. If it isn't, an error occurred and the callee should return.

The return value should not be used if an error occurred.

```c
int callee(hbe_err_t *hbe_err, int a, int b) {
  int meaning_of_life = error_prone(hbe_err, "Yes");
  if (*hbe_err != NULL) {
    // An error occurred, $meaning_of_life is unreliable
    return 0;
  }

  return 3;
}
```

To simplify this code, a macro is available:

```c
int callee(hbe_err_t *hbe_err, int a, int b) {
  int meaning_of_life = HBE_CATCH(error_prone, hbe_err, "Yes");
  /* Translates to:
  int meaning_of_life = error_prone(hbe_err, "Yes");
  if (*hbe_err != NULL) {
    return 0;
  }
  */

  return 3;
}
```

If the return type is `void`, use `HBE_CATCH_V` instead.
If there is a cleanup section, use `HBE_CATCH_F`.

## Returning with cleanup

Use the macro `HBE_RETURN_F` to set the return value and go to the cleanup section:

```c
int fn(hbe_err_t *hbe_err) {
  int rv = 0;

  HBE_RETURN_F(1);
  /* Translates to:
  rv = 1;
  goto finally;
  */

  finally:
    return rv;
}
```

## Top-level error handler

At the very root, where the call to the first error-prone function resides, create a variable with type `hbe_err_t` set to `NULL` on the stack, and pass a reference to it:

After the call, if an error occurred, the variable will be set to a value other than `NULL`.

```c
int main(void) {
  hbe_err_t err = NULL;
  fn(&err);
  if (err != NULL) {
    // An error occurred
  }
}
```
