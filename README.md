# GL-Zigbee SDK for GL.iNet zigbee gateway

GL-Zigbee SDK is developed and provided by GL-iNet Technology as a part of the zigbee solution. The SDK encapsulates the communication between the gateway and the zigbee module, allowing third-party developers to focus on the business layer rather than the communication layer.



## Support device

- [Convexa-S (GL-S1300)](https://www.gl-inet.com/products/gl-s1300/)

- [Velica (GL-B2200)](https://www.gl-inet.com/products/gl-b2200/)

## System requirements

- x86_64 platform

- Ubuntu or another linux distro

Compiling under Windows can be done using the Windows Subsystem For Linux (WSL) with Ubuntu installed to it. Follow the guide bellow, installing Ubuntu 18.04 LTS from the Microsoft Store: 

 https://docs.microsoft.com/en-us/windows/wsl/install-win10

## Preparing your build environment

Before using the SDK, you need to install the Openwrt SDK environment.

https://github.com/gl-inet/sdk

# Downloads

```
$ git clone https://github.com/gl-inet/gl-zigbee-sdk.git
```

# Compiling

Place your package in `openwrt-sdk/<version>/<target>/package/`, then run:

```
$ cd sdk/<version>/<target>/
$ make package/<package_name>/compile V=s
```

For example, to compile the zigbee SDK for the S1300

```
$ cd sdk/qsdk53/ipq806x/
$ make package/gl-zigbee-sdk/{clean,compile} V=s
```

If there is a problem with missing dependent libraries during compilation, follow these steps

```
$ ./scripts/feeds update -f
$ ./scripts/feeds install <dependency_name>
```

# Directory Structure

```
|—— Makefile
|
|—— LICENSE								
|
|—— VERSION_FILE							
|
|—— README							
|
|—— doc									               # document
|	|—— Zbtool User Guide
|	|—— GL-Zigbee SDK User Guide
|
|—— files
|	|—— gl-zbtool.init				      	# configuration file
|
|—— src
    |—— components
    |   |—— dev_mgr
    |   |—— log
    |  	|—— thread
    |
    |—— daemon 						         	# zb daemon
    |   |—— daemon
    |   |—— zbdriver
    |       |——silabs				     	# silabs SDK
    |
    |—— include               	# header file 
    |
    |—— lib                  		# zb api lib
    |   |—— libglzbapi.h
    |   |—— libglzbapi.c
    |
    |—— project             			# user application file
    |   |—— demo.c	        				# demo file
    |
    |—— tool                     	
    |   |—— cli.c						# debug tool – zbtool
    |
    |—— Makefile

```

# API Refrence

Look at the doc/GL-Zigbee SDK User Guide. It contains all detail about the API.

