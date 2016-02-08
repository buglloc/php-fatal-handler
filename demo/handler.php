<?php

class ErrorHandler
{
    protected $reservedMemory = null;

    public function register()
    {
        set_error_handler(array($this, 'handleError'), E_ALL | E_STRICT);
        set_exception_handler(array($this, 'handleException'));
        fatal_error_handler(array($this, 'handleFatalError'));
        $this->reservedMemory = str_repeat(' ', 1024 * 20);
    }

    public function handleFatalError($code, $mess, $file = '', $line = 0, $context = array())
    {
        $this->reservedMemory = null;
        $exception = new \ErrorException($mess, $code, 1, $file, $line);
        $this->handleErrorException($exception);
        die('Ooops');
    }

    public function handleError($code, $mess, $file = '', $line = 0, $context = array())
    {
        if (!(error_reporting() & $code))
            return true;

        $exception = new \ErrorException($mess, $code, 0, $file, $line);
        $this->handleErrorException($exception);
        return true;
    }

    public function handleException(\Exception $ex)
    {
        $message = sprintf('EXCEPTION %s %s in %s:%d',
            get_class($exception), $exception->getMessage(),
            $exception->getFile(), $exception->getLine());

        $message .= PHP_EOL.$exception->getTraceAsString();

        $this->save($message);
    }

    public function handleErrorException(\ErrorException $exception)
    {
        $message = sprintf('%s: %s in %s:%d',
            $this->errorCodeName($exception->getCode()), $exception->getMessage(),
            $exception->getFile(), $exception->getLine());

        $exception_trace = $exception->getTraceAsString();
        $exception_trace = substr($exception_trace, strpos($exception_trace, PHP_EOL) + 1);
        $message .= PHP_EOL.$exception_trace;

        $this->save($message);
    }

    protected function save($message)
    {
        error_log($message);
    }

    protected static function errorCodeName($code)
    {
        switch ($code) {
            case E_ERROR:
                $result = 'E_ERROR';
                break;
            case E_WARNING:
                $result = 'E_WARNING';
                break;
            case E_PARSE:
                $result = 'E_PARSE';
                break;
            case E_NOTICE:
                $result = 'E_NOTICE';
                break;
            case E_CORE_ERROR:
                $result = 'E_CORE_ERROR';
                break;
            case E_CORE_WARNING:
                $result = 'E_CORE_WARNING';
                break;
            case E_COMPILE_ERROR:
                $result = 'E_COMPILE_ERROR';
                break;
            case E_COMPILE_WARNING:
                $result = 'E_COMPILE_WARNING ';
                break;
            case E_USER_ERROR  :
                $result = 'E_USER_ERROR';
                break;
            case E_USER_WARNING:
                $result = 'E_USER_WARNING';
                break;
            case E_USER_NOTICE:
                $result = 'E_USER_NOTICE';
                break;
            case E_STRICT:
                $result = 'E_STRICT';
                break;
            case E_DEPRECATED:
                $result = 'E_DEPRECATED';
                break;
            case E_USER_DEPRECATED:
                $result = 'E_USER_DEPRECATED';
                break;
            default:
                $result = 'UNKNOWN';
        }

        return $result;
    }
}
