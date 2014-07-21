<?php
    $token = $_GET["token"]; 
    $filename = $token.".json";
    echo $filename; 
    echo "Test"; 
    $file = fopen("$filename", "w"); 
    fwrite($file, $_POST['data']); 
    fclose($file); 


?>