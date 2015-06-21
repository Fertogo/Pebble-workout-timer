<?php
    // https://developer.getpebble.com/sdk/migration-guide/#pebblekit-js-account-token
    function newToOld($token) {
        $array = str_split($token);
        return implode('', array_map(function($char, $i) {
            return ($char !== '0' || $i % 2 == 1) ? $char : '';
        }, $array, array_keys($array)));
    }

    $token = $_GET["token"];

    if (!ctype_alnum ($token)) return; // Sanitize token (only alphanumeric)

    $filename = "userworkouts/".$token.".json";
    if (file_exists($filename)) {
        $contents = file_get_contents($filename);
        echo $contents;
    }
    else { // try old token
        $old_token = newToOld($token);
        $old_filename = "userworkouts/".$old_token.".json";
        if (file_exists($old_filename)) {
            $contents = file_get_contents($old_filename);
            rename($old_filename, $filename);
            echo $contents;
        }
        else echo '{"workouts":[]}';
    }



?>
