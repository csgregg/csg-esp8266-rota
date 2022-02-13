
<?php

    $output = shell_exec('pwd');
    echo "<pre>$output</pre>";

    $output = shell_exec('cd');

    $output = shell_exec('pwd');
    echo "<pre>$output</pre>";

    $output = shell_exec('uapi VersionControlDeployment create repository_root=csg-esp8266-rota');
    echo "<pre>$output</pre>";
  
?>
