<?php
    $githubApiUrl = "https://api.github.com/repos/csgregg/csg-esp8266-rota/releases/latest";
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_URL, $githubApiUrl);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array(
    'User-Agent: ESP8266'));
    $result = curl_exec($ch);
    //echo $json;
    curl_close($ch);
    $json = json_decode($result);
    $latestTag = $json->tag_name;
    $currentTag = $_GET["tag"];
    if ($latestTag != $currentTag) {
        $binPath = $json->assets[0]->browser_download_url;
        // the file you want to send
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $binPath);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        $out = curl_exec($ch);
        curl_close($ch);
        // Set header for binary
        header('Content-type: application/octet-stream');
        header('Content-disposition: attachment; filename="firmware.bin"');
        header('Content-Transfer-Encoding: binary');
        header("Content-Length: ".strlen($out));
        echo $out;
    } else {
       header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
    }
    exit();
?>