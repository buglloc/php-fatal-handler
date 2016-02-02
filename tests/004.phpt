--TEST--
Checks trace available
--SKIPIF--
<?php if (!extension_loaded("fatal_handler")) print "skip"; ?>
--FILE--
<?php
fatal_error_handler(function($code, $message, $file, $line) {
  echo "Args: \n";
  var_dump(func_get_args());
  echo "Trace: \n";
  debug_print_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS);
});

function test_error() {
  fatal();
}

test_error();
?>
--EXPECTF--
Args: 
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(34) "Call to undefined function fatal()"
  [2]=>
  string(%d) "%s/004.php"
  [3]=>
  int(%d)
}
Trace: 
#0  {closure}(%S) called at [%s/004.php:%d]
#1  test_error() called at [%s/004.php:%d]
