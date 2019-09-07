# ESP8266 Remote OTA CI using PlatformIO, Github and Travis
A basic example of how to set up Remote Over-The-Air firmware updates for ESP8266 with Continuous Integration. Releases tagged in GitHub matching x.x.x release tags will be built by Travis, and the binary stored back as an asset in the release in GitHub.
Since GitHub uses SSL and it's not supported in the ESP8266, the HTTPUpdate library in the ESP8266 code calls a redirect script to deliver the binary for the firmware update. 
 
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
 - Add README.md and LICENSE as necessary

Useful videos - [https://www.youtube.com/watch?v=I7WfxhF2wEg](https://www.youtube.com/watch?v=I7WfxhF2wEg)

**Travis-CI**

 - Link Travis-CI.org account to GitHub
 - Install Ruby and Travis locally
 - Create encrypted OAuth token using `travis setup releases` and seed the token in the [travis.yml](https://github.com/csgregg/csg-esp8266-rota/blob/master/.travis.yml) file. Update the rest of the YML file
 - The binary is created with a dynamic filename

Useful links -

 - [http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/](http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/)
 - [https://prof.bagneris.net/posts/2017/github-releases-travis-ci-en.html](https://prof.bagneris.net/posts/2017/github-releases-travis-ci-en.html)



<!--stackedit_data:
eyJoaXN0b3J5IjpbMTM3MTE1Nzc0Miw3MTQxMjkxOTYsMjgyNj
U0OTMyLDkzNTc4MjM0NiwxMDAwODg2NjQwLC01NjgwODg5Mjks
LTExNzUzMTM0NTksLTE2NTE4NTMwXX0=
-->