<?php

    $DEBUG = false;

    // Get parameters
    if(!empty($_GET["debug"])) $DEBUG = ($_GET["debug"] == "true");
    if(!empty($_GET["tag"])) $currentTag = $_GET["tag"];
    if(!empty($_GET["repo"])) $repoName = $_GET["repo"];
    if(!empty($_GET["asset"])) $imageFilePre = $_GET["asset"];


    // Check for repo 
    if( $DEBUG && empty($repoName) ) {
        echo "Missing GitHub Repo name";
        exit;
    }
    elseif( $DEBUG ) echo nl2br("Repo: $repoName\r\n");
    elseif( empty($repoName) ){
        header($_SERVER["SERVER_PROTOCOL"].' 400 Missing GitHub repo name', true, 400);
        exit;
    }

    $githubApiUrl = "https://api.github.com/repos/${repoName}/releases/latest";

    if( $DEBUG ) echo nl2br("Repo API URL: $githubApiUrl\r\n");

    // Get API response fpr lastest release
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_URL, $githubApiUrl);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('User-Agent: ESP8266'));
    $result = curl_exec($ch);

    curl_close($ch);
    $json = json_decode($result);

    // Check valid response    
    if( $DEBUG && !empty($json->message) ){
        echo "Repo error: $json->message";
        exit;
    }
    elseif( !$DEBUG && !empty($json->message) ){
        header($_SERVER["SERVER_PROTOCOL"]." 400 GitHub API error: $json->message", true, 400);
        exit;
    }


    // Get tag of latest release
    $latestTag = $json->tag_name;

    // return latest version if no ?tag=XXXX
    if( !$DEBUG && empty($currentTag) ){
        echo $latestTag;
        exit;
    }

    if( $DEBUG && !empty($latestTag) ) echo nl2br("Latest release: $latestTag\n\r");
    if( $DEBUG && !empty($currentTag) ) echo nl2br("Release requested: $currentTag\n\r");


    // Check image file
    if( $DEBUG && empty($imageFilePre) ) {
        echo "Missing asset name prefix";
        exit;
    }
    elseif( !$DEBUG && empty($imageFilePre) ){
        header($_SERVER["SERVER_PROTOCOL"].' 400 Missing asset name prefix', true, 400);
        exit;
    }

    // Build image file name
    $imageFile = "$imageFilePre$latestTag.bin";

    if( $DEBUG ) echo nl2br("Image file: $imageFile\r\n");


    // Find asset download URL
    foreach ($json->assets as $asset) if( $asset->name == $imageFile ) $binPath = $asset->browser_download_url;
    
    if( $DEBUG && empty($binPath) ){
        echo "GitHub asset not found";
        exit;
    }
    elseif( !$DEBUG && empty($binPath) ){
        header($_SERVER["SERVER_PROTOCOL"].' 404 GitHub asset not found', true, 404);
        exit;
    }
   

    // Check to see if download needed
    if( $DEBUG && ($latestTag == $currentTag) ) echo "No new release available";
    elseif( !$DEBUG && ($latestTag == $currentTag) ) header($_SERVER["SERVER_PROTOCOL"].' 304 No new release available', true, 304);
    elseif( $DEBUG ) echo "Serving file: $binPath";
    else {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $binPath);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        $out = curl_exec($ch);
        curl_close($ch);

        // Set header for binary
        header('Content-type: application/octet-stream');
        header("Content-disposition: attachment; filename=$imageFile");
        header('Content-Transfer-Encoding: binary');
        header("Content-Length: ".strlen($out));
        echo $out;
    }
    exit;
?>