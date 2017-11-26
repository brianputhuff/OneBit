## OneBit

This program accepts a .BMP format image file as input and exports a black and white .BMP format image. The program renders output as black and white pixels only and uses dithering to simulate shades without gray scale. The program also generates an output text file that depicts the input image using ASCII characters to simulate gray scale. This program uses the EasyBMP Cross-Platform Windows Bitmap Library (http://easybmp.sourceforge.net/). You will need this to compile.

## Compiling

I have included a makefile to the open repository for OneBit, however you will still need to download the EasyBMP library (http://easybmp.sourceforge.net/). Once downloaded, put the files found in the repository along with a folder containing the EasyBMP library files into your destination location. The structure should resemble the following:
```
onebit/
       EasyBMP/
               EasyBMP.cpp
               EasyBMP.h
               EasyBMP_BMP.h
               EasyBMP_DataStructures.h
               EasyBMP_VariousBMPutilities.h

       main.cpp

       makefile
```
If you have the files arranged as above, simply execute:

`make`

That should compile and create an executable for OneBit. If you want to compile the program yourself (without using make), enter this on the Linux command line:

`g++ main.cpp ./EasyBMP/EasyBMP.cpp -o onebit`

## Running

After you get the executable, simply run it with one command line argument, the name of the .BMP image file you want to convert. For example:

`./onebit filename.bmp`

The program will generate the black and white dithered version with the format:

`filename_BW.bmp`

The ASCII text output will be saved as:

`filename_GS.txt`

