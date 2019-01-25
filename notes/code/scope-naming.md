# Scope naming

## Public

```c
int hb_sub_function_name(int a, int b);
```

## Internal use only

Used across multiple files but should only be used by this project's code.

```c
int _hb_sub_function_name(int a, int b);
```

## Within same file only

```c
// Don't declare in header file
static int _function_name(int a, int b) {}
```
