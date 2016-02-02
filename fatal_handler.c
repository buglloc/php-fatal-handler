/*
  +----------------------------------------------------------------------+
  | Author: Andrew Krasichkov                                            |
  +----------------------------------------------------------------------+
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "SAPI.h"
#include "Zend/zend.h"
#include "ext/standard/info.h"
#include "php_fatal_handler.h"


ZEND_DECLARE_MODULE_GLOBALS(fatal_handler)

/* error handling redirection functions */
ZEND_API void (*orig_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
ZEND_API void php_feh_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fatal_error_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, fatal_error_handler)
ZEND_END_ARG_INFO()
/* }}} */


/* {{{ fatal_handler_functions[]
 *
 * Every user visible function must have an entry in fatal_handler_functions[].
 */
static const zend_function_entry fatal_handler_functions[] = {
	PHP_FE(fatal_error_handler,				arginfo_fatal_error_handler)
	{NULL, NULL, NULL}	/* Must be the last line in fatal_handler_functions[] */
};
/* }}} */

/* {{{ fatal_handler_module_entry
 */
zend_module_entry fatal_handler_module_entry = {
    STANDARD_MODULE_HEADER,
    "fatal_handler",
    fatal_handler_functions,
    PHP_MINIT(fatal_handler), /* PHP_MINIT */
    NULL, /* PHP_MSHUTDOWN */
    PHP_RINIT(fatal_handler), /* PHP_RINIT */
    PHP_RSHUTDOWN(fatal_handler), /* PHP_RSHUTDOWN */
    PHP_MINFO(fatal_handler),
    PHP_FEH_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FATAL_HANDLER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(fatal_handler)
#endif


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fatal_handler)
{
	orig_error_cb = zend_error_cb;
	zend_error_cb = php_feh_error_cb;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(fatal_handler)
{
	FEH_G(user_error_handler) = NULL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(fatal_handler)
{
	if (FEH_G(user_error_handler)) {
		FREE_ZVAL(FEH_G(user_error_handler));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fatal_handler)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Fatal handler support", "enabled");
	php_info_print_table_end();
}
/* }}} */


/* {{{ proto string fatal_error_handler(string error_handler)
   Sets a user-defined error handler function.*/
PHP_FUNCTION(fatal_error_handler)
{
	zval *error_handler;
	char *error_handler_name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &error_handler) == FAILURE) {
		return;
	}

	if (FEH_G(user_error_handler)) {
		return;
	}

	if (!zend_is_callable(error_handler, 0, &error_handler_name TSRMLS_CC)) {
		zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
				   get_active_function_name(TSRMLS_C), error_handler_name?error_handler_name:"unknown");
		efree(error_handler_name);
		return;
	}
	efree(error_handler_name);

	ALLOC_ZVAL(FEH_G(user_error_handler));
	*FEH_G(user_error_handler) = *error_handler;
	zval_copy_ctor(FEH_G(user_error_handler));
	INIT_PZVAL(FEH_G(user_error_handler));

	RETURN_TRUE;
}
/* }}} */


ZEND_API void php_feh_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	int r;
	zval ***params;
	zval *retval;
	zval *z_error_type, *z_error_message, *z_error_filename, *z_error_lineno;
	zval *user_error_handler;

	if (type != E_ERROR
		|| !FEH_G(user_error_handler)
		|| !php_get_module_initialized()
	) {
		orig_error_cb(type, error_filename, error_lineno, format, args);
		return;
	}

	ALLOC_INIT_ZVAL(z_error_message);
	ALLOC_INIT_ZVAL(z_error_type);
	ALLOC_INIT_ZVAL(z_error_filename);
	ALLOC_INIT_ZVAL(z_error_lineno);

	Z_STRLEN_P(z_error_message) = vspprintf(&Z_STRVAL_P(z_error_message), 0, format, args);
	Z_TYPE_P(z_error_message) = IS_STRING;

	Z_LVAL_P(z_error_type) = type;
	Z_TYPE_P(z_error_type) = IS_LONG;

	if (error_filename) {
		ZVAL_STRING(z_error_filename, error_filename, 1);
	} else {
		ZVAL_STRING(z_error_filename, "unknown", 1);
	}

	Z_LVAL_P(z_error_lineno) = error_lineno;
	Z_TYPE_P(z_error_lineno) = IS_LONG;

	params = (zval ***) emalloc(sizeof(zval **)*4);
	params[0] = &z_error_type;
	params[1] = &z_error_message;
	params[2] = &z_error_filename;
	params[3] = &z_error_lineno;

	/* We call user func only ONE time! */
	user_error_handler = FEH_G(user_error_handler);
	FEH_G(user_error_handler) = NULL;

	r = call_user_function_ex(CG(function_table), NULL, user_error_handler, &retval, 4, params, 1, NULL TSRMLS_CC);

	efree(params);
	zval_ptr_dtor(&z_error_message);
	zval_ptr_dtor(&z_error_type);
	zval_ptr_dtor(&z_error_filename);
	zval_ptr_dtor(&z_error_lineno);
	zval_ptr_dtor(&user_error_handler);

	if (r != SUCCESS && !EG(exception)) {
		/* The user error handler failed, use built-in error handler */
		orig_error_cb(type, error_filename, error_lineno, format, args);
	}

	EG(exit_status) = 255;
	if (!SG(headers_sent) &&
		SG(sapi_headers).http_response_code == 200
	) {
		sapi_header_line ctr = {0};

		ctr.line = "HTTP/1.0 500 Internal Server Error";
		ctr.line_len = sizeof("HTTP/1.0 500 Internal Server Error") - 1;
		sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
	}
	
	/* restore memory limit */
	zend_set_memory_limit(PG(memory_limit));
	zend_objects_store_mark_destructed(&EG(objects_store) TSRMLS_CC);
	zend_bailout();
}
