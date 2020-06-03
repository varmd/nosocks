# Nosocks

Control access to the internet for non-static apps on Linux.

----
## what is nosocks?
Nosocks uses library preloading to limit access to the internet for apps that are dynamically linked to libc - or do not have libc statically linked. 

---
## usage
1. Download zip file from github to Archlinux.
2. Unzip, cd to zipped directory, in the terminal run makepkg, and then pacman -U nosocks-*
3. As root user edit app names in the /etc/nosocks/nosocks.conf
4. Close and restart apps to check internet access. By default nosocks does not apply to the root user
5. For wine apps you need to add /usr/bin/wineserver, and the exe name separately
----
## Logging
Export NOSOCKS_DEBUG=2 to see verbose messages

Export NOSOCKS_DEBUG_FILE=yourfile to log messages to a file


----
## changelog
* 1-2020 Initial release
* 5-2020 Add wine exe support


