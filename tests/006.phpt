--TEST--
Check w/ exception
--SKIPIF--
<?php if (!extension_loaded("fatal_handler")) print "skip"; ?>
--FILE--
<?php
fatal_error_handler(function($code, $message, $file, $line) {
  throw new \ErrorException($message, 0, $code, $file, $line);
});

function foo() {
  some_fatal();
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught exception 'ErrorException' with message 'Call to undefined function some_fatal()' in %s/006.php:%d
Stack trace:
#0 %s/006.php(%d): {closure}(%d, '%s', '%s', %d)
#1 %s/006.php(%d): foo()
#2 {main}
  thrown in %s/006.php on line %d
