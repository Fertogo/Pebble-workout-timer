<?php
    $token = $_GET["token"]; 
    $filename = $token.".json";
    if (file_exists($filename)) { 
        //$handle = fopen($filename, "r");
        //$contents = fread($handle, filesize($filename));
        //fclose($handle);
        $contents = file_get_contents($filename); 

        echo $contents; 
    }
    else echo "false";    
?>