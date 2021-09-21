@echo off
set compiler=cl.exe
set arg=/W4 /Zi /EHsc
set src=main.cpp
set libs=/link opencv_core453.lib opencv_imgcodecs453.lib opencv_imgproc453.lib opencv_highgui453.lib

%compiler% %arg% %src% %libs%

main.exe