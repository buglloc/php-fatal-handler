PHP_ARG_ENABLE(fatal_handler, whether to enable smd support,
[  --enable-fatal-handler   Enable fatal error handler support])

PHP_ARG_WITH(fatal-handler-sanitize, whether to enable AddressSanitizer for Fatal Error Handle,
[  --with-fatal-handler-sanitize Build smd with AddressSanitizer support], no, no)

if test "$PHP_FATAL_HANDLER" != "no"; then
  if test "$PHP_FATAL_HANDLER_SANITIZE" != "no"; then
    EXTRA_LDFLAGS="-lasan"
  EXTRA_CFLAGS="-fsanitize=address -fno-omit-frame-pointer"
  PHP_SUBST(EXTRA_LDFLAGS)
    PHP_SUBST(EXTRA_CFLAGS)
  fi

  PHP_NEW_EXTENSION(fatal_handler, fatal_handler.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi