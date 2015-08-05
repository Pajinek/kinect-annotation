# Annotation kinect data - RGB, Depth, XML #

This project is given over to linux platforms and opensource kinnect driver.

## Basic functionality ##

  * play/pase video
  * move position in video
  * annotation frames in video
  * record depth and video data (using opencv)

## Roadmap ##

  * save/load data to XML (libxml)
  * edit/delete annotation data
  * save sceleton (x,y,z) data
  * revord audio

## Problems ##

  * Fedora - https://bugzilla.redhat.com/show_bug.cgi?id=812628

## Instalation ##

  * Fedora 17:

>> ldd annotation-gtk | sed s/\(.**\)//g | sed s/^.**\=\>//g | xargs rpm -qf | uniq |  sed s/[-].**$//g**

libfreenect
libusb1
opencv
libxml2
zlib
glibc
gtk2
atk
glib2
pango
gdk
cairo
pango
freetype
fontconfig
glib2
libstdc++
libgcc
glibc
tbb
libjpeg
libpng
libtiff
jasper
ilmbase
OpenEXR
ilmbase
gstreamer
gstreamer
libdc1394
libv4l
openni
glibc
libX11
libXfixes
libXext
libXrender
libXinerama
libXi
libXrandr
libXcursor
libXcomposite
libXdamage
libffi
libselinux
glibc
pixman
expat
ilmbase
orc
libraw1394
libv4l
tinyxml
libxcb
libv4l
libXau

  * Ubuntu 12.10
```
apt-get install g++ subversion libopencv-* freenect-*
```

## Sreenshost ##

![http://kinect-annotation.googlecode.com/files/screeenshot1.png](http://kinect-annotation.googlecode.com/files/screeenshot1.png)