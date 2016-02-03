/*
  +----------------------------------------------------------------------+
  | Author: Andrew Krasichkov buglloc@yandex.ru                          |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_FATAL_HANDLER_H
#define PHP_FATAL_HANDLER_H

extern zend_module_entry fatal_handler_module_entry;
#define phpext_fatal_handler_ptr &fatal_handler_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef PHP_WIN32
#   define PHP_FATAL_HANDLER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_FATAL_HANDLER_API __attribute__ ((visibility("default")))
#else
#   define PHP_FATAL_HANDLER_API
#endif

#ifdef ZTS
#define FEH_G(v) TSRMG(fatal_handler_globals_id, zend_fatal_handler_globals *, v)
#else
#define FEH_G(v) (fatal_handler_globals.v)
#endif

#define PHP_FEH_VERSION "0.2"

ZEND_BEGIN_MODULE_GLOBALS(fatal_handler)
  zval *user_error_handler;
ZEND_END_MODULE_GLOBALS(fatal_handler)

PHP_MINFO_FUNCTION(fatal_handler);
PHP_MINIT_FUNCTION(fatal_handler);
PHP_RINIT_FUNCTION(fatal_handler);
PHP_RSHUTDOWN_FUNCTION(fatal_handler);

PHP_FUNCTION(fatal_error_handler);

#endif  /* PHP_FATAL_HANDLER_H */
