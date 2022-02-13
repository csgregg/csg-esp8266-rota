
<?php

// From https://lejenome.tik.tn/post/github-push-webhook-implementation

// Secret Random Code You set on github webhook settings
const SECRET_TOKEN = 'password';

function execPrint($command) {
    $result = array();
    exec($command, $result);
    print("<pre>");
    foreach ($result as $line) {
        print($line . "\n");
    }
    print("</pre>");
}

if(strcasecmp($_SERVER['REQUEST_METHOD'], 'POST') != 0){
        throw new Exception('Request method must be POST');
}

if (! in_array($_SERVER['HTTP_X_GITHUB_EVENT'], ['push', 'ping'])) {
    throw new Exception('Request event should be either "push" or "ping"');
}

$hash =  hash_hmac('sha1', file_get_contents('php://input'), SECRET_TOKEN, false);

if ($_SERVER['HTTP_X_HUB_SIGNATURE'] == 'sha1=' . $hash) {

    $user = trim(shell_exec('whoami'));
    try {
        execPrint("git --verbose--git-dir=/home/$user/repo/csg-esp8266-rota/.git --work-tree=/home/$user/repo/csg-esp8266-rota pull");
     //   error_log($tmp);
        $tmp = shell_exec("uapi VersionControlDeployment create repository_root=/home/$user/repo/csg-esp8266-rota");
        error_log($tmp);
    } catch (Exception $e) {
        $tmp = "ERROR!\n";
        $tmp = 'Caught exception: '.  $e->getMessage(). "\n";
    }

} else {
    throw new Exception('Wrong signature hash!');
}
?>