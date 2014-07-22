<?php
    echo 'PHP Oprnrf';
    $token = $_GET["token"]; 
    $filename = $token.".json";
    $file = fopen("$filename", "w"); 
    fwrite($file, $_POST['data']); 
    fclose($file); 
    echo 'File Updated';
?>