
<?php

// From https://lejenome.tik.tn/post/github-push-webhook-implementation

// Secret Random Code You set on github webhook settings
const SECRET_TOKEN = 'password';

if(strcasecmp($_SERVER['REQUEST_METHOD'], 'POST') != 0){
        throw new Exception('Request method must be POST');
}

if (! in_array($_SERVER['HTTP_X_GITHUB_EVENT'], ['push', 'ping'])) {
    throw new Exception('Request event should be either "push" or "ping"!');
}

$hash =  hash_hmac('sha1', file_get_contents('php://input'), SECRET_TOKEN, false);

if ($_SERVER['HTTP_X_HUB_SIGNATURE'] == 'sha1=' . $hash) {

    $user = trim(shell_exec('whoami'));

    // Run it
    try {
    //    $tmp = shell_exec("git --git-dir=/home/$user/repo/csg-esp8266-rota/.git pull");
        $tmp = shell_exec("uapi VersionControlDeployment create repository_root=/home/$user/repo/csg-esp8266-rota");
    } catch (Exception $e) {
        $tmp = "ERROR!\n";
        $tmp = 'Caught exception: '.  $e->getMessage(). "\n";
    }
    // Output
    echo "sh> $cmd\n";
    echo htmlentities(trim($tmp)) . "\n";

} else {
    throw new Exception('Wrong signature hash!');
}
?>