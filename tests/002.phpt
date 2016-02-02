--TEST--
Simple checks error handling
--SKIPIF--
<?php if (!extension_loaded("fatal_handler")) print "skip"; ?>
--FILE--
<?php
fatal_error_handler(function($code, $message, $file, $line) {
  var_dump(func_get_args());
});

some_fatal();
?>
--EXPECTF--
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(39) "Call to undefined function some_fatal()"
  [2]=>
  string(%d) "%s/002.php"
  [3]=>
  int(%d)
}
