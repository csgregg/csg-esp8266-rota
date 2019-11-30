[![Status](https://travis-ci.org/csgregg/csg-esp8266-rota.svg?branch=master)](https://travis-ci.org/csgregg/csg-esp8266-rota) [![Status](http://img.shields.io/travis/csgregg/csg-esp8266-rota/master.svg?style=flat)](https://travis-ci.org/csgregg/csg-esp8266-rota "See test builds")


# ESP8266 Remote OTA Update using PlatformIO, Github and Travis
A basic example of how to set up Remote Over-The-Air firmware updates for ESP8266 using Travis Continuous Integration.

Releases tagged in GitHub matching x.x.x release tags will be built by Travis, and the binary stored back as an asset in the release in GitHub.

Since GitHub uses SSL and it's not supported in the ESP8266, the HTTPUpdate library in the ESP8266 code calls a redirect script to deliver the binary for the firmware update from the release asset. 

## Setup
**PlatformIO**

Add PlatformIO extension to VSCode. Install Git.
 - Create project in PlatformIO
 - Setup build environments - see [platformio.ini](platformio.ini)
 -  Add script to dynamically name binary output file - [extra_script.py](extra_script.py)

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
 - Seed the encrypted OAuth token in the [travis.yml](.travis.yml) file using `travis setup releases` from the local repo folder
 - Update the rest of the YML file


Useful links -

 - [http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/](http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/)
 - [https://prof.bagneris.net/posts/2017/github-releases-travis-ci-en.html](https://prof.bagneris.net/posts/2017/github-releases-travis-ci-en.html)

**Host Redirect**

Host the redirect script somewhere visible - [csg-esp8266-rota.php](server/csg-esp8266-rota.php)


<!--stackedit_data:
eyJoaXN0b3J5IjpbMTcwNzA3Mzc2LC0zODYyNTM3NTIsLTExNT
kwOTc4NTEsMTk4MjU2NjcxOSw3MTQxMjkxOTYsMjgyNjU0OTMy
LDkzNTc4MjM0NiwxMDAwODg2NjQwLC01NjgwODg5MjksLTExNz
UzMTM0NTksLTE2NTE4NTMwXX0=
-->
