Update(20171105)
New Feature: Add Scale bar to dm image, resize dm image with adjusting scale bar
Implementation: Caculate one suitable scale bar and add to dm image
		When resize an dm image, recalculate the scale bar

Update(20171105)
New Feature: Read and Display DM3 and DM4 file
Implementation: Extend current DM3 file reader for reading DM4 file.
		Refactor current logic for better understanding

Update(20171101)
New Feature: Show OpenCV Image in MFC dialog
Implementation: Add new class MatCImageConverter, which could convert OpenCV Image to MFC Image,
		MFC Image could be inserted to MFC window directly.


------
This demo application includes 3 functions:

#1 Show false color picker dialog and test the selected color scale with lenna.png.
#2 Combine two one-channel images to one single HSV image, in the demo,
   perform fourier transform to lenna.png and show combined image.
#3 Show color picker dialog for scale bar.


To compile this project, please config it according to the real environment:

#1 Make sure OpenCV has been installed correctly.

In my implementation, OpenCV-3.3.1(x64)is used, other versions should also be OK.
Add binary directory to system variable "PATH", in my case it is
"C:\Users\yrao\Downloads\opencv\build\x64\vc14\bin"

#2 Add additional include directories to the VS project, in my case, it is

C:\Users\yrao\Downloads\opencv\build\include
C:\Users\yrao\Downloads\opencv\build\include\opencv
C:\Users\yrao\Downloads\opencv\build\include\opencv2

#3 Add additional library directories to the VS project, in my case, it is

C:\Users\yrao\Downloads\opencv\build\x64\vc14\lib

#4 Add additional dependencies to the VS project, in my case, it is
opencv_world331d.lib  (For Debug)
opencv_world331.lib  (For Release)

Any question, please contact raoyonghui0630@gmail.com


