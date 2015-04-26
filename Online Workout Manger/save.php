<?php
    echo("Starting PHP");
    $token = $_GET["token"]; 
    $filename = "userworkouts/".$token.".json";
    $file = fopen("$filename", "w"); 
    fwrite($file, $_POST["data"]); 
    fclose($file); 
    echo("PHP done")
?>