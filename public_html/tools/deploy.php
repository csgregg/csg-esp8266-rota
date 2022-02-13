
<?php

    shell_exec('cd && pwd');
    $output = shell_exec('uapi VersionControlDeployment create repository_root=csg-esp8266-rota');
    echo "<pre>$output</pre>";
  
?>
