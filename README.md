
Description: Plant Health and Photosynthetic Activity Quantification using Near Infra Red Images
Language:    C++
Date:        17th December 2014
Author:      Nick Arini
References:  
                http://publiclab.org/wiki/near-infrared-camera
                http://publiclab.org/notes/cfastie/04-20-2013/superblue
                http://www.fsnau.org/downloads/Understanding_the_Normalized_Vegetation_Index_NDVI.pdf
                http://www.raspberrypi.org/whats-that-blue-thing-doing-here/
                http://infragram.org/
Usage: 
                This program will take an infrablue image (from a NoIR camera with blue filter) in PNG format.
                It will output a Normalised Difference Vegetation Index (NDVI) Image scaled 0-255
                Additionally it will auto threshold this image into vegetation/non vegetation and sum over the NDVI 
                values for the vegetation to produce an overall relative metric for vegetation health/photosynthetic 
                activity

To Build Type:

autoreconf --install

./configure

make install
