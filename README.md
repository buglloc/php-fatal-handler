# PHP Fatal Error Handler [![Build Status](https://travis-ci.org/buglloc/php-fatal-handler.svg?branch=master)](https://travis-ci.org/buglloc/php-fatal-handler)

### Intro
К сожалению, PHP 5.x (в PHP 7.x есть [engine exceptions](https://wiki.php.net/rfc/engine_exceptions_for_php7)) не позволяет установить кастомный обработчик для ошибок типа `E_ERROR`, `E_PARSE`, `E_CORE_ERROR`, `E_CORE_WARNING`, `E_COMPILE_ERROR` и `E_COMPILE_WARNING`, обоснованно считая это не безопасным. Из них наиболее любопытен [`E_ERROR`](http://php.net/manual/en/errorfunc.constants.php#errorfunc.constants.errorlevels.e-error), типичное решение этой проблемы - зарегистрировать shutdown-функцию ([register_shutdown_function](http://php.net/manual/en/function.register-shutdown-function.php)) и там кетчить. И все не так плохо, если вам не интересен stack trace, ибо его на момент вызова shutdown-функции уже нет :(

Этот небольшой PHP extension позволяет прокинуть в user-space ошибки типа `E_ERROR` по аналогии с остальными, что дает возможность обработать их нужным образом (i.e. сохранить) и "умереть".

### Installation
Подобно всем другим расширениям:
```bash
# Создадим временный каталог, куда склонируем расширение
mkdir ~/tmp && cd ~/tmp
git clone https://github.com/buglloc/php-fatal-handler.git
cd php-fatal-handler
# Подготовим окружение для PHP-расширения
phpize
# Собираем
./configure && make
# На всякий случай можно запустить тесты
make test
# Установим
sudo make install
# И напоследок включаем расширение
sudo bash -c 'echo "extension=fatal_handler.so" > "/etc/php/conf.d/fatal_handler.ini"'
```

### Usage
После установки добавится функция `fatal_error_handler`, семантика и назначение схоже с [set_error_handler](http://php.net/manual/en/function.set-error-handler.php):
```
bool fatal_error_handler ( callable $error_handler )

void error_handler ( int $errno , string $errstr [, string $errfile [, int $errline [, array $errcontext ]]] )
```
Пример использование:
```php
fatal_error_handler(function($code, $message, $file, $line) {
  echo "Args: \n";
  var_dump(func_get_args());
  echo "Trace: \n";
  debug_print_backtrace();
});
function test_error() {
  fatal();
}
test_error();
```
Вывод:
```
Args: 
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(34) "Call to undefined function fatal()"
  [2]=>
  string(26) "/home/buglloc/tmp/test.php"
  [3]=>
  int(10)
}
Trace: 
#0  {closure}(1, Call to undefined function fatal(), /home/buglloc/tmp/test.php, 10) called at [/home/buglloc/tmp/test.php:10]
#1  test_error() called at [/home/buglloc/tmp/test.php:13]
```
Больше примеров можно посмотреть в [тестах](/tree/master/tests) или [демо](/demo) с реализацией простого хендлера ошибок/исключений
