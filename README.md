# ESP8266 Remp OTA CI using PlatformIO, Github and Travis
 A basic example of how to set up Over-The-Air firmware updates for ESP8266 with Continuous Integration.
 
Uses:
 - PlatformIO IDE
 - GitHub public repos
 - Travis-CI.org for CI

## Setup
**PlatformIO**
Add PlatformIO extension to VSCode. Install Git.
 - Create project in PlatformIO
 - Setup build environments - see [platformio.ini](https://github.com/csgregg/csg-esp8266-rota/blob/master/platformio.ini)

**Github**
 - Create repository in GitHub
 - Make public
 - Skip step to import an existing repository
 - Use command line to add remote to local Git repo

Useful videos - [https://www.youtube.com/watch?v=I7WfxhF2wEg](https://www.youtube.com/watch?v=I7WfxhF2wEg)

**Travis-CI**

 - Link Travis-CI.org account to GitHub
 - Install Ruby and Travis locally
 - Create encrypted OAuth token using `travis setup releases` and seed the token in the travis.yml file.

Useful links -

 - [http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/](http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/)
 - [https://prof.bagneris.net/posts/2017/github-releases-travis-ci-en.html](https://prof.bagneris.net/posts/2017/github-releases-travis-ci-en.html)

<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE2MjIzNTgxNzAsNzE0MTI5MTk2LDI4Mj
Y1NDkzMiw5MzU3ODIzNDYsMTAwMDg4NjY0MCwtNTY4MDg4OTI5
LC0xMTc1MzEzNDU5LC0xNjUxODUzMF19
-->