--TEST--
Checks error handling w/ OOM
--SKIPIF--
<?php if (!extension_loaded("fatal_handler")) print "skip"; ?>
--FILE--
<?php
fatal_error_handler(function($code, $message, $file, $line) {
  var_dump(func_get_args());
});

$str = '';
while (true) {
	$str .= str_repeat('aa', 60000);
}
?>
--EXPECTF--
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(%d) "Allowed memory size of %d bytes exhausted (tried to allocate %d bytes)"
  [2]=>
  string(%d) "%s/003.php"
  [3]=>
  int(%d)
}
