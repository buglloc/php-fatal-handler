--TEST--
Check for Fatal Error Handler presence
--FILE--
<?php
if (extension_loaded("fatal_handler"))
	echo "FEH extension is available";
?>
--EXPECT--
FEH extension is available