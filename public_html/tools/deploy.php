
<?php

    $user = shell_exec('whoami');

    $commmand = 'uapi VersionControlDeployment create repository_root=/home' & $user & '/repo/csg-esp8266-rota';
    echo "<pre>$command</pre>";

 //   $output = shell_exec('uapi VersionControlDeployment create repository_root=csg-esp8266-rota');
 //   echo "<pre>$output</pre>";
  
?>
