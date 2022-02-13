<?php

/* Remote Updater Library

MIT License

Copyright (c) 2019 Chris Gregg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-----------------------------------------------------------------------------

Provides interfaces to GitHub API for Remote Updater library, as ESP8266
doesn't support HTTPS.

Usage:
    debug=true              Sets debug mode
    repo=<repo>             Name of GitHub repo
    user=<user>             GitHub user
    (token=<token>)         GitHub API OAuth Token - only for private repos

Mode 1 - Get latest release details
    (tag)                   Omitted

    Returns JSON detailing latest release

Mode 2 - Get all releases
    tag=all
    max=<max>               Sets max number of releases to return - default 30

    Returns JSON detailing all releases (up to maxtags)

Mode 3 - Serve latest release
    tag=latest
    asset=<assetname>       Appended by release tag

    Serves the asset image file from the latest release

Mode 4 - Serve specific release
    tag=<release>
    asset=<assetname>       Appended by release tag

    Serves the asset image file from the specific release


*/

    $DEBUG = false;

    // Get parameters
    if( !empty($_GET["debug"]) ) $DEBUG = ($_GET["debug"] == "true");
    if( !empty($_GET["tag"]) ) $requestedTag = $_GET["tag"];
    if( !empty($_GET["repo"]) ) $repoName = $_GET["repo"];
    if( !empty($_GET["asset"]) ) $imageFilePre = $_GET["asset"];
    if( !empty($_GET["max"]) ) $maxReleases = $_GET["max"];
    if( !empty($_GET["token"]) ) $token = $_GET["token"];
    if( !empty($_GET["user"]) ) $user = $_GET["user"];
    if( !empty($_GET["type"]) ) $type = $_GET["type"];

    if( $DEBUG ) echo nl2br("ESP8266 GitHub Updater - Asset Fetch Service\r\n");

    // Set mode
    if( empty($requestedTag) ) $mode = 1;
    elseif( $requestedTag == "all" ) $mode = 2;
    elseif( $requestedTag == "latest" ) $mode = 3;
    else $mode = 4;

    if( $DEBUG ) echo nl2br("Mode: $mode\r\n");

    // Check for user 
    if( $DEBUG && empty($user) ){
        echo "Missing GitHub API user";
        exit;
    }
    elseif( $DEBUG ) echo nl2br("User: $user\r\n");
    elseif( empty($user) ){
        header($_SERVER["SERVER_PROTOCOL"].' 400 Missing GitHub API user', true, 400);
        exit;
    }

    // Check for repo 
    if( $DEBUG && empty($repoName) ){
        echo "Missing GitHub Repo name";
        exit;
    }
    elseif( $DEBUG ) echo nl2br("Repo: $repoName\r\n");
    elseif( empty($repoName) ){
        header($_SERVER["SERVER_PROTOCOL"].' 400 Missing GitHub repo name', true, 400);
        exit;
    }

    // Check for token 
    if( $DEBUG && empty($token) ) echo nl2br("Public repo only\r\n");
    elseif( $DEBUG ) echo nl2br("Token: $token\r\n");

    // Check for type 
    if( $DEBUG && empty($type) ) echo nl2br("Uncompressed\r\n");
    elseif( $DEBUG ) echo nl2br("Type: $type\r\n");

    // Default max releases to 30
    if( empty($maxReleases) ) $maxReleases = 30;

    // Check for filename 
    if( $mode == 3 || $mode == 4 ) {
        if( $DEBUG && empty($imageFilePre) ) {
            echo "Missing asset file name";
            exit;
        }
        elseif( empty($imageFilePre) ) {
            header($_SERVER["SERVER_PROTOCOL"].' 400 Missing asset file name', true, 400);
            exit;
        }
    }

    // Get details of all releases

    $githubApiUrl = "https://api.github.com/repos/${user}/${repoName}/releases";

    if( $DEBUG ) echo nl2br("Repo API URL: $githubApiUrl\r\n");

    // Get API response fpr lastest release
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_URL, $githubApiUrl);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('User-Agent: ESP8266'));
    if( !empty($token) ) curl_setopt($ch, CURLOPT_USERPWD, "$user:$token");
    $result = curl_exec($ch);

    curl_close($ch);
    $json = json_decode($result);

    // Check valid response    
    if( $DEBUG && !empty($json->message) ) {
        echo "Repo error: $json->message";
        exit;
    }
    elseif( !$DEBUG && !empty($json->message) ) {
        header($_SERVER["SERVER_PROTOCOL"]." 400 GitHub API error: $json->message", true, 400);
        exit;
    }


    // Build JSON for response

    class githubapiresponse {
        public $repo;
        public $releases;
    }

    class githubrelease {
        public $tag;
        public $date;
        public $assets;
    }

    class githubasset {
        public $name;
        public $size;
        public $URL;
    }

    $response = new githubapiresponse();

    $response->repo = $repoName;

    $releases = array();

    $count = 0;

    foreach( $json as $key=>$release){

        $count++;

        $thisrelease = new githubrelease();
        $thisrelease->tag = $release->tag_name;
        $thisrelease->date = $release->published_at;

        if( $count == 1 ) $latestTag = $release->tag_name;
     
        if( !empty($type) && $type == 'gz' ) $extsn = ".bin.gz";
        else $extsn = ".bin";

        // Check to see if this is the release we want
        if( $mode == 3 ) $assetName = "$imageFilePre$release->tag_name$extsn";
        if( $mode == 4 ) $assetName = "$imageFilePre$requestedTag$extsn";

        if( $DEBUG ) echo "Asset name: $assetName\r\n";

        $assets = array();

        foreach( $release->assets as $asset ) {

            $thisasset = new githubasset();
            $thisasset->name = $asset->name;
            $thisasset->size = $asset->size;
            $thisasset->URL = $asset->browser_download_url;

            // Check for the asset we want
            if( ($mode == 3) || ($mode == 4) ) if($asset->name == $assetName) $binPath = $asset->browser_download_url;

            array_push($assets, $thisasset);
        }

        $thisrelease->assets = $assets;

        array_push( $releases, $thisrelease );

        // Stop if just want latest
        if( $mode == 1 || $mode == 3 ) break;

        // Stop if found asset
        if( !empty($binPath) ) break;

        // Stop if max reached
        if( $mode == 2 && $count >= $maxReleases ) break;
        
    }

    $response->releases = $releases;

    if( $DEBUG ) {
        echo nl2br("Latest release: $latestTag\n\r");
        if( $mode != 1 ) echo nl2br("Requested release: $requestedTag\n\r");
        echo nl2br("Max repleases: $maxReleases\n\r");
    }

 
    if( $mode == 3 || $mode == 4 ) {
        if( $DEBUG && empty($binPath) && !empty($requestedTag) && $requestedTag != "all" ) {
            echo "GitHub asset not found";
            exit;
        }
        elseif( !$DEBUG && empty($binPath) && !empty($requestedTag) && $requestedTag != "all" ) {
            header($_SERVER["SERVER_PROTOCOL"].' 404 GitHub asset not found', true, 404);
            exit;
        }
    }

    // Output JSON
    if( $mode == 1 || $mode == 2 ) {

        $jsonResponse = json_encode( $response );

        if( $DEBUG ) echo nl2br("JSON Response: \n\r$jsonResponse");
        else echo $jsonResponse;

        exit;
    }
    else {

        // Serve file

        if( $DEBUG ) echo "Serving file: $binPath";
        else {
            $ch = curl_init();
            curl_setopt($ch, CURLOPT_URL, $binPath);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_HEADER, 0);
            curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
            if( !empty($token) ) curl_setopt($ch, CURLOPT_USERPWD, "$user:$token");
            $out = curl_exec($ch);
            curl_close($ch);
    
            // Set header for binary
            header('Content-type: application/octet-stream');
            header("Content-disposition: attachment; filename=$assetName");
            header('Content-Transfer-Encoding: binary');
            header("Content-Length: ".strlen($out));
            echo $out;
        }
    }

?>