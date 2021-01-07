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

typedef struct js_min_buf_metadata {
    napi_ref src_buf_ref;
} js_min_buf_metadata;

void js_cfg_finalizer(napi_env env, void* finalize_data, void* _finalize_hint) {
  ffi_drop_cfg((Cfg const*) finalize_data);
}

void js_min_buf_finalizer(napi_env env, void* _finalize_data, void* finalize_hint) {
  js_min_buf_metadata* metadata = (js_min_buf_metadata*) finalize_hint;
  assert_ok(napi_delete_reference(env, metadata->src_buf_ref));
  free(metadata);
}

void js_copy_min_buf_finalizer(napi_env env, void* _finalize_data, void* finalize_hint) {
  free(finalize_hint);
}

static inline void throw_js_ffi_error(napi_env env, ffi_error const* min_err) {
  napi_value js_min_err_msg;
  assert_ok(napi_create_string_utf8(env, (char const*) min_err->message, min_err->message_len, &js_min_err_msg));
  napi_value js_min_err;
  assert_ok(napi_create_error(env, NULL, js_min_err_msg, &js_min_err));
  napi_value js_min_err_pos;
  assert_ok(napi_create_int64(env, min_err->position, &js_min_err_pos));
  assert_ok(napi_set_named_property(env, js_min_err, "position", js_min_err_pos));
  assert_ok(napi_throw(env, js_min_err));
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

  // Get `minifyJs` property.
  napi_value minify_js_value;
  if (napi_get_named_property(env, obj_arg, "minifyJs", &minify_js_value) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get minifyJs property"));
    return undefined;
  }
  bool minify_js;
  if (napi_get_value_bool(env, minify_js_value, &minify_js) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get minifyJs boolean property"));
    return undefined;
  }

  // Get `minifyCss` property.
  napi_value minify_css_value;
  if (napi_get_named_property(env, obj_arg, "minifyCss", &minify_css_value) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get minifyCss property"));
    return undefined;
  }
  bool minify_css;
  if (napi_get_value_bool(env, minify_css_value, &minify_css) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get minifyCss boolean property"));
    return undefined;
  }

  Cfg const* cfg = ffi_create_cfg(minify_js, minify_css);

  napi_value js_cfg;
  if (napi_create_external(env, (void*) cfg, js_cfg_finalizer, NULL, &js_cfg) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to create return value"));
    return undefined;
  }
  return js_cfg;
}

napi_value node_method_minify_in_place(napi_env env, napi_callback_info info) {
  napi_value undefined = get_undefined(env);
  napi_value min_buf_rv = undefined;

  bool buffer_arg_ref_set = false;
  napi_ref buffer_arg_ref;
  js_min_buf_metadata* min_buf_meta = NULL;
  ffi_error const* min_err = NULL;

  size_t argc = 2;
  napi_value argv[2];
  napi_value _this;
  void* _data;

  // Get the arguments.
  if (napi_get_cb_info(env, info, &argc, argv, &_this, &_data) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to get callback info"));
    goto rollback;
  }
  napi_value buffer_arg = argv[0];
  napi_value js_cfg_arg = argv[1];

  // Ensure buffer lives along side minified buffer (both point to same memory).
  if (napi_create_reference(env, buffer_arg, 1, &buffer_arg_ref) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to create reference for source buffer"));
    goto rollback;
  }
  buffer_arg_ref_set = true;

  // Get pointer to bytes in buffer.
  void* buffer_data;
  size_t buffer_len;
  if (napi_get_buffer_info(env, buffer_arg, &buffer_data, &buffer_len) != napi_ok || buffer_data == NULL) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to read source buffer"));
    goto rollback;
  }

  // Get Cfg.
  void* cfg_raw;
  if (napi_get_value_external(env, js_cfg_arg, &cfg_raw) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get configuration"));
    goto rollback;
  }
  Cfg const* cfg = (Cfg const*) cfg_raw;

  // Run minifier in place.
  size_t min_len;
  min_err = ffi_in_place(buffer_data, buffer_len, cfg, &min_len);
  if (min_err != NULL) {
    throw_js_ffi_error(env, min_err);
    goto rollback;
  }

  // Create minified buffer with underlying source memory but minified length.
  min_buf_meta = assert_malloc(sizeof(js_min_buf_metadata));
  min_buf_meta->src_buf_ref = buffer_arg_ref;
  if (napi_create_external_buffer(env, min_len, buffer_data, js_min_buf_finalizer, min_buf_meta, &min_buf_rv) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to create minified buffer"));
    goto rollback;
  }

  goto cleanup;

rollback:
  if (buffer_arg_ref_set) {
    // Release source buffer.
    assert_ok(napi_delete_reference(env, buffer_arg_ref));
  }
  free(min_buf_meta);

cleanup:
  if (min_err != NULL) {
    ffi_drop_ffi_error(min_err);
  }

  return min_buf_rv;
}

napi_value node_method_minify(napi_env env, napi_callback_info info) {
  napi_value undefined = get_undefined(env);
  napi_value min_buf_rv = undefined;

  void* src_data_copy = NULL;
  ffi_error const* min_err = NULL;

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

  size_t src_data_len;
  if (napi_is(env, src_arg, napi_is_buffer)) {
    // Get pointer to bytes in buffer.
    void* buffer_data;
    if (napi_get_buffer_info(env, src_arg, &buffer_data, &src_data_len) != napi_ok || buffer_data == NULL) {
      assert_ok(napi_throw_type_error(env, NULL, "Failed to read source buffer"));
      goto rollback;
    }
    src_data_copy = assert_malloc(src_data_len);
    memcpy(src_data_copy, buffer_data, src_data_len);
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
  }

  // Get Cfg.
  void* cfg_raw;
  if (napi_get_value_external(env, js_cfg_arg, &cfg_raw) != napi_ok) {
    assert_ok(napi_throw_type_error(env, NULL, "Failed to get configuration"));
    goto rollback;
  }
  Cfg const* cfg = (Cfg const*) cfg_raw;

  // Run minifier in place.
  size_t min_len;
  min_err = ffi_in_place(src_data_copy, src_data_len, cfg, &min_len);
  if (min_err != NULL) {
    throw_js_ffi_error(env, min_err);
    goto rollback;
  }

  // Create minified buffer with copied memory.
  if (napi_create_external_buffer(env, min_len, src_data_copy, js_copy_min_buf_finalizer, src_data_copy, &min_buf_rv) != napi_ok) {
    assert_ok(napi_throw_error(env, NULL, "Failed to create minified buffer"));
    goto rollback;
  }

  goto cleanup;

rollback:
  free(src_data_copy);

cleanup:
  if (min_err != NULL) {
    ffi_drop_ffi_error(min_err);
  }

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
  define_method(env, exports, "minifyInPlace", node_method_minify_in_place);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, node_module_init)
