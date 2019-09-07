# Basic ESP8266 OTA CI using PlatformIO, Github and Travis
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
		 - Local builds
		 - Travis builds 

**Github**
 - Create repository in GitHub 
		 - Make public
		 - Skip step to import an existing repository
 - Use command line to add remote to local Git repo

Useful videos - [https://www.youtube.com/watch?v=I7WfxhF2wEg](https://www.youtube.com/watch?v=I7WfxhF2wEg)

**Travis-CI**

 - Install Ruby and Travis locally
 - Create encrypted OAuth token using travis setup releases and seed the 

Useful links -

 - [http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/](http://blog.code4hire.com/2016/06/adding-github-token-to-travis-ci-configuration/)


<!--stackedit_data:
eyJoaXN0b3J5IjpbMTI3Nzg4OTExMCw5MzU3ODIzNDYsMTAwMD
g4NjY0MCwtNTY4MDg4OTI5LC0xMTc1MzEzNDU5LC0xNjUxODUz
MF19
-->