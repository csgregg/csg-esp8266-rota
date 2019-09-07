# Basic ESP8266 OTA CI using PlatformIO, Github and Travis
 A basic example of how to set up Over-The-Air firmware updates for ESP8266 with Continuous Integration.
 
Uses:
 - PlatformIO IDE
 - GitHub public repos
 - Travis-CI.org for CI

## Setup
**PlatformIO**

 - Create project in PlatformIO
 - Setup build environments
		 - Local builds
		 - Travis builds 

> [common]
> .. any common options such as include libraries
>
>[env]
>platform = espressif8266
>board = d1_mini
>framework = arduino
[env
    



<!--stackedit_data:
eyJoaXN0b3J5IjpbMjEyNTIyMDgwNywtNTY4MDg4OTI5LC0xMT
c1MzEzNDU5LC0xNjUxODUzMF19
-->