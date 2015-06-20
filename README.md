# nitepy

This project opens the functionality of nite to python

In addition to this, it performs many other handy actions for LILI such as facial recognition and shirt data retrevial.

It can also record faces!  Find the variable named "capture" where it is set (line 74) and set it to true instead of false.
Then, check name.txt.  Set the new user's name and id number. (the name is just to name the files, lili will not speak this name.)
Compile the code, and then rename the nitepy.dll in the release folder to be nitepyCapture.dll and copy this into the lssrobotwin32 folder.
(nitepyCapture.dll might already be up to date in the LILI project folder.)
In nitepy.py, change line 11 to: lib=ctypes.CDLL('nitepyCapture')
Then, run KinectMonitor by itself.  Stand in front of it to get face images (both either side or front) recorded. 
Don't do side and front under the same ID/name, that doesn't work.   Side pictures of people need to be differently named and id'd. 
If you want the side pictures to be recognized as front pictures, you can put these relations with the personMappings Dictionary in KinectMonitor.py.

After you have recorded your pictures, img.txt is supposed to hold the information that needs to be in faces.txt in order for lili to use the new images.
This doesn't work a lot of the time though :s
I'm not yet sure why this is, but hey, ctrl+f "capture" and check out what's going on, it's pretty simple. 

the format of what is suppose to be in faces.txt is as follows:

somefolder/name1_1.bmp;ID1
somefolder/name1_2.bmp;ID1
somefolder/name2_1.bmp;ID2
somefolder/name2_2.bmp;ID2

where name1 and name2 are different names of people, and ID1 and ID2 are different integers.

Use these entries to control what images LILI uses to recognize somebody.

um.... you know where to find me. 
