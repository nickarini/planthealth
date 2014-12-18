Planthealth
============

### Description: 

Plant Health and Photosynthetic Activity Quantification using Near Infra Red Images

Language:    C++

Date:        17th December 2014

Author:      Nick Arini

### References:  

http://publiclab.org/wiki/near-infrared-camera
http://publiclab.org/notes/cfastie/04-20-2013/superblue
http://www.fsnau.org/downloads/Understanding_the_Normalized_Vegetation_Index_NDVI.pdf
http://www.raspberrypi.org/whats-that-blue-thing-doing-here/
http://infragram.org/

### Usage: 

This program will take an infrablue image (from a NoIR camera with blue filter) in PNG format.
It will optionally output a Normalised Difference Vegetation Index (NDVI) Image scaled 0-255
Additionally it will auto threshold this image into vegetation/non vegetation and sum over the NDVI 
values for the vegetation to produce an overall relative metric for vegetation health/photosynthetic 
activity

```
   Usage: planthealth [-h] [-d] [-o output.png] input.png
	-h Display this help message.
	-d Verbose output.
	-o Output the Scaled NDVI image to [output].
	   Input and Output images must be PNG Format.
```

```./planthealth -d -o ndvi.png infrablue.png```

The sample image infrablue.png is included in the repository:

![infrablue.png](https://github.com/nickarini/planthealth/raw/master/infrablue.png)


The output should look like this:

![ndvi.png](https://github.com/nickarini/planthealth/raw/master/ndvi.png)


To output the thresholded bitmap instead of the scaled NDVI image use the -b flag:

```./planthealth -d -b -o bitmap.png infrablue.png```


The bitmap should look like this:

![bitmap.png](https://github.com/nickarini/planthealth/raw/master/bitmap.png)


### Installation:

The only dependency is lodepng which can be found here:
http://lodev.org/lodepng/

It is included here for simplicity. 

To Build Type:

```
   autoreconf --install
   ./configure
   make install
```
