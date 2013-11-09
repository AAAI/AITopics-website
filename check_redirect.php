<?php

$redirected = "false";

/*
$ip = $_SERVER['REMOTE_ADDR'];
if(copy('/tmp/redirect-log', '/tmp/redirect-log-copy')) {
    $f = fopen('/tmp/redirect-log', 'w');
    if($f !== FALSE && flock($f, LOCK_EX | LOCK_NB)) {
        $fcopy = fopen('/tmp/redirect-log-copy', 'r');
        if($fcopy) {
            while(($line = fgets($fcopy)) !== false) {
                if($line == "$ip\n") {
                    $redirected = "true";
                } else {
                    fwrite($f, $line);
                }
            }
            fclose($fcopy);
        }
        flock($f, LOCK_UN);
        fclose($f);
    }
}
*/

print $redirected;
