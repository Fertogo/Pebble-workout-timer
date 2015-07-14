<?php
    echo("Starting PHP");
    $token = $_GET["token"];
    if (!ctype_alnum ($token)){
        echo("Invalid Token");
        return;
    }

    $filename = "userworkouts/".$token.".json";
    $file = fopen("$filename", "w");
    fwrite($file, $_POST["data"]);
    fclose($file);
    echo("PHP done")
?>
