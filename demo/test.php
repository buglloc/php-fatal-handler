<?php
require_once __DIR__ . '/handler.php';

$handler = new ErrorHandler;
$handler->register();

function foo() {
    fatal();
}

function bar() {
    foo();
}

// warning
strpos();

// fatal
bar();

__halt_compiler();
Expected output:
E_WARNING: strpos() expects at least 2 parameters, 0 given in /home/buglloc/work/projects/php-fatal-handler/demo/test.php:16
#1 /home/buglloc/work/projects/php-fatal-handler/demo/test.php(16): strpos()
#2 {main}
E_ERROR: Call to undefined function fatal() in /home/buglloc/work/projects/php-fatal-handler/demo/test.php:9
#1 /home/buglloc/work/projects/php-fatal-handler/demo/test.php(13): foo()
#2 /home/buglloc/work/projects/php-fatal-handler/demo/test.php(19): bar()
#3 {main}
Ooops