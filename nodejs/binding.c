#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAPI_VERSION 1
#include <node_api.h>

typedef struct Cfg Cfg;

#include <minify_html_ffi.h>

static inline void* assert_malloc(size_t bytes) {
  void* ptr = malloc(bytes);
  if (ptr == NULL) {
    fprintf(stderr, "[Node.js minify-html] Internal assertion error: failed to allocate memory\n");
    exit(67);
  }
  return ptr;
}

static inline void assert_ok(napi_status status) {
  if (status != napi_ok) {
    fprintf(stderr, "[Node.js minify-html] Internal assertion error: N-API status not OK\n");
    exit(67);
  }
}

static inline napi_value get_undefined(napi_env env) {
  napi_value undefined;
  assert_ok(napi_get_undefined(env, &undefined));
  return undefined;
}

typedef napi_status (*napi_is_pred)(napi_env env, napi_value value, bool* result);

static inline bool napi_is(napi_env env, napi_value value, napi_is_pred pred) {
  bool res;
  assert_ok(pred(env, value, &res));
  return res;
}

void js_cfg_finalizer(napi_env env, void* finalize_data, void* _finalize_hint) {
  ffi_drop_cfg((Cfg const*) finalize_data);
}

void js_output_buf_finalizer(napi_env env, void* _finalize_data, void* finalize_hint) {
  ffi_output* metadata = (ffi_output*) finalize_hint;
  ffi_drop_output(metadata);
}

napi_value node_method_create_configuration(napi_env env, napi_callback_info info) {
  napi_value undefined = get_undefined(env);

  size_t argc = 1;
  napi_value argv[1];
  napi_value _this;
  void* _data;

  // Get the arguments.
  if (napi_get_cb_info(env, info, &argc, argv, &_this, &_data) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to get callback info"));
    return undefined;
  }
  napi_value obj_arg = argv[0];

#define GET_CFG_PROP(prop) \
  bool prop = false; \
  napi_value prop##_value; \
  if (napi_get_named_property(env, obj_arg, #prop, &prop##_value) == napi_ok) { \
    /* It's OK if this fails. */ napi_get_value_bool(env, prop##_value, &prop); \
  }

  GET_CFG_PROP(do_not_minify_doctype);
  GET_CFG_PROP(ensure_spec_compliant_unquoted_attribute_values);
  GET_CFG_PROP(keep_closing_tags);
  GET_CFG_PROP(keep_comments);
  GET_CFG_PROP(keep_html_and_head_opening_tags);
  GET_CFG_PROP(keep_spaces_between_attributes);
  GET_CFG_PROP(minify_css);
  GET_CFG_PROP(minify_js);
  GET_CFG_PROP(remove_bangs);
  GET_CFG_PROP(remove_processing_instructions);

  Cfg const* cfg = ffi_create_cfg(
    do_not_minify_doctype,
    ensure_spec_compliant_unquoted_attribute_values,
    keep_closing_tags,
    keep_comments,
    keep_html_and_head_opening_tags,
    keep_spaces_between_attributes,
    minify_css,
    minify_js,
    remove_bangs,
    remove_processing_instructions
  );

  napi_value js_cfg;
  if (napi_create_external(env, (void*) cfg, js_cfg_finalizer, NULL, &js_cfg) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to create return value"));
    return undefined;
  }
  return js_cfg;
}

napi_value node_method_minify(napi_env env, napi_callback_info info) {
  napi_value undefined = get_undefined(env);
  napi_value min_buf_rv = undefined;

  void* src_data_copy = NULL;

  size_t argc = 2;
  napi_value argv[2];
  napi_value _this;
  void* _data;

  // Get the arguments.
  if (napi_get_cb_info(env, info, &argc, argv, &_this, &_data) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to get callback info"));
    goto rollback;
  }
  napi_value src_arg = argv[0];
  napi_value js_cfg_arg = argv[1];

  void* src_data;
  size_t src_data_len;
  if (napi_is(env, src_arg, napi_is_buffer)) {
    // Get pointer to bytes in buffer.
    if (napi_get_buffer_info(env, src_arg, &src_data, &src_data_len) != napi_ok || src_data == NULL) {
      assert_ok(napi_throw_type_error(env, NULL, "Failed to read source buffer"));
      goto rollback;
    }
  } else {
    // Assume string.
    if (napi_get_value_string_utf8(env, src_arg, NULL, 0, &src_data_len) != napi_ok) {
      assert_ok(napi_throw_type_error(env, NULL, "Failed to read source string"));
      goto rollback;
    }
    src_data_copy = assert_malloc(src_data_len + 1);
    size_t bytes_copied;
    if (napi_get_value_string_utf8(env, src_arg, src_data_copy, src_data_len + 1, &bytes_copied) != napi_ok
      || bytes_copied != src_data_len) {
      assert_ok(napi_throw_error(env, NULL, "Failed to copy source string"));
      goto rollback;
    }
    src_data = src_data_copy;
  }

  // Get Cfg.
  void* cfg_raw;
  if (napi_get_value_external(env, js_cfg_arg, &cfg_raw) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get configuration"));
    goto rollback;
  }
  Cfg const* cfg = (Cfg const*) cfg_raw;

  // Run minifier.
  ffi_output const* output = ffi_minify(src_data, src_data_len, cfg);

  // Create minified buffer with copied memory.
  if (napi_create_external_buffer(env, output->len, output->data, js_output_buf_finalizer, (void*) output, &min_buf_rv) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to create minified buffer"));
    goto rollback;
  }

  goto cleanup;

rollback:

cleanup:
  free(src_data_copy);
  return min_buf_rv;
}

static inline void define_method(napi_env env, napi_value exports, char const* name, napi_callback cb) {
  napi_value js_fn;
  assert_ok(napi_create_function(env, name, NAPI_AUTO_LENGTH, cb, NULL, &js_fn));
  assert_ok(napi_set_named_property(env, exports, name, js_fn));
}

napi_value node_module_init(napi_env env, napi_value exports) {
  define_method(env, exports, "createConfiguration", node_method_create_configuration);
  define_method(env, exports, "minify", node_method_minify);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, node_module_init)
