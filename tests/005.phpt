--TEST--
Checks trace available in include
--SKIPIF--
<?php if (!extension_loaded("fatal_handler")) print "skip"; ?>
--INI--
allow_url_include=1
--FILE--
<?php
fatal_error_handler(function($code, $message, $file, $line) {
  echo "Args: \n";
  var_dump(func_get_args());
  echo "Trace: \n";
  debug_print_backtrace();
});

include 'data:,<?php function throw_fatal() {$obj = null;$obj->call();} throw_fatal();';
?>
--EXPECTF--
Args: 
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(%d) "Call to a member function call() on %r(null|a non-object)%r"
  [2]=>
  string(77) "data:,<?php function throw_fatal() {$obj = null;$obj->call();} throw_fatal();"
  [3]=>
  int(1)
}
Trace: 
#0  {closure}(%S) called at [data:,<?php function throw_fatal() {$obj = null;$obj->call();} throw_fatal();:1]
#1  throw_fatal() called at [data:,<?php function throw_fatal() {$obj = null;$obj->call();} throw_fatal();:1]
#2  include(data:,<?php function throw_fatal() {$obj = null;$obj->call();} throw_fatal();) called at [%s/005.php:%d]