
<?php

    $output = shell_exec('cd $HOME/repo/csg-esp8266-rota && git pull && pwd');
    echo "<pre>$output</pre>";

    $output = shell_exec('uapi VersionControlDeployment create repository_root=$HOME/repo/csg-esp8266-rota');
    echo "<pre>$output</pre>";
  
?>
