<?php
    $token = $_GET["token"]; 
    $filename = "userworkouts/".$token.".json";
    if (file_exists($filename)) { 
        $contents = file_get_contents($filename); 
        echo $contents; 
    }
    else echo '{"workouts":[]}';    
?>