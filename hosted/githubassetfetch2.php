
<?php

// NEED TO DOCUMENT USE

    $DEBUG = false;

    // Get GitHub OAuth token
    include 'githuboauthtoken.php';
    // NEED TO DESCRIBE FORMAT OF THIS

    // Get parameters
    if( !empty($_GET["debug"]) ) $DEBUG = ($_GET["debug"] == "true");
    if( !empty($_GET["tag"]) ) $requestedTag = $_GET["tag"];
    if( !empty($_GET["repo"]) ) $repoName = $_GET["repo"];
    if( !empty($_GET["asset"]) ) $imageFilePre = $_GET["asset"];

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
    
    $githubApiUrl = "https://api.github.com/repos/${repoName}/releases";

    if( $DEBUG ) echo nl2br("Repo API URL: $githubApiUrl\r\n");

    // Get API response fpr lastest release
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_URL, $githubApiUrl);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('User-Agent: ESP8266'));
    curl_setopt($ch, CURLOPT_USERPWD, "$githubusername:$githuboauthtoken");
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
        public $URL;
    }

    $reponse = new githubapiresponse();

    $reponse->repo = $repoName;

    $releases = array();

    foreach( $json as $key=>$release){

        $thisrelease = new githubrelease();
        $thisrelease->tag = $release->tag_name;
        $thisrelease->date = $release->published_at;

        $assets = array();

        foreach ($release->assets as $asset){

            $thisasset = new githubasset();
            $thisasset->name = $asset->name;
            $thisasset->URL = $asset->browser_download_url;

            array_push($assets, $thisasset);
        }

        $thisrelease->assets = $assets;

        array_push( $releases, $thisrelease );
        
    }

    $reponse->releases = $releases;

    echo json_encode( $reponse );

    exit;




        // Get tag of latest release
        if( empty($requestedTag) || $requestedTag == "Latest" ) { 
            $latestTag = $json->tag_name;
            if( $DEBUG ) echo nl2br("Latest release: $latestTag\n\r");
        }  
    
    

    // Find asset download URL
    foreach ($json->assets as $asset) if( $asset->name == $imageFile ) $binPath = $asset->browser_download_url;
        
    if( $DEBUG && empty($binPath) ) {
        echo "GitHub asset not found";
        exit;
    }
    elseif( !$DEBUG && empty($binPath) ) {
        header($_SERVER["SERVER_PROTOCOL"].' 404 GitHub asset not found', true, 404);
        exit;
    }

    // Download file
    if( $DEBUG ) echo "Serving file: $binPath";
    else {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $binPath);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_USERPWD, "$githubusername:$githuboauthtoken");
        $out = curl_exec($ch);
        curl_close($ch);

        // Set header for binary
        header('Content-type: application/octet-stream');
        header("Content-disposition: attachment; filename=$imageFile");
        header('Content-Transfer-Encoding: binary');
        header("Content-Length: ".strlen($out));
        echo $out;
    }
?>