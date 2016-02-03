--TEST--
Check w/ exception and user exception handler
--SKIPIF--
<?php if (!extension_loaded("fatal_handler")) print "skip"; ?>
--FILE--
<?php
fatal_error_handler(function($code, $message, $file, $line) {
  throw new \ErrorException($message, 0, $code, $file, $line);
});

set_exception_handler(function(\ErrorException $exception) {
  printf("Exception: %s\nTrace:\n%s",
        $exception->getMessage(), $exception->getTraceAsString());
});

function foo() {
  some_fatal();
}

foo();
?>
--EXPECTF--
Exception: Call to undefined function some_fatal()
Trace:
#0 %s/007.php(%d): {closure}(%d, '%s', '%s', %d)
#1 %s/007.php(%d): foo()
#2 {main}
