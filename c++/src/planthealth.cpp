/*--------------------------------------------------------------------------------------------------------------
   Module:      planthealth.cpp
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
  --------------------------------------------------------------------------------------------------------------*/

// Includes
#include <getopt.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h" // The only non standard dependency is lightweight lodepng module: http://lodev.org/lodepng/


// Definitions:
static int debug=0;


// Load a PNG File from Disk
// The pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA
void loadPNG(const char* filename, std::vector<unsigned char>& image, int& Width, int& Height)
{
  unsigned width, height;

  //decode
  unsigned error = lodepng::decode(image, width, height, filename);

  Width = (int) width;
  Height = (int) height;

  //if there's an error, display it
  if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}


// Save a PNG Image to the supplied filename
// The image argument has width * height RGBA pixels or width * height * 4 bytes
void savePNG(const char* filename, const std::vector<unsigned char>& image, unsigned width, unsigned height)
{
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}


// Otsu Method for Automatic Thresholding
// from: http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html
int otsu_threshold(const std::vector<float>& scaled, int Width, int Height)
{
  // Calculate histogram
  std::vector<int> histogram;
  if(histogram.size() != 256) // check it is the right size
    histogram.resize(256);
  for(int i=0; i<256; i++) // Initialise Histogram bins to zero
    histogram[i] = 0;
  for (int dy=0; dy<Height; dy++){ // Now loop through the image and 
    for (int dx=0; dx<Width; dx++){
      int index = scaled[dy * Width + dx]; // Find the bin
      if (index > 255 ) // make sure we are not out of bounds
	printf("index too big: %d", index);
      if (index < 0 )
	printf("index too small: %d", index);
      histogram[ index ]++; // Increment the bin frequency
    }
  }
    
  // Total number of pixels
  int total = scaled.size();

  // Now calculate the Otsu Threshold
  float sum = 0.0;
  for (int t=0 ; t<256 ; t++) sum += t * histogram[t];
  
  float sumB = 0.0;
  int wB = 0;
  int wF = 0;
 
  float varMax = 0.0;
  int threshold = 0;
  
  for (int t=0 ; t<256 ; t++) {
    wB += histogram[t];               // Weight Background
    if (wB == 0) continue;
    
    wF = total - wB;                 // Weight Foreground
    if (wF == 0) break;
    
    sumB += (float) (t * histogram[t]);
    
    float mB = sumB / wB;            // Mean Background
    float mF = (sum - sumB) / wF;    // Mean Foreground
   
    // Calculate Between Class Variance
    float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);
   
    // Check if new maximum found
    if (varBetween > varMax) {
      varMax = varBetween;
      threshold = t;
    }
  }
  return threshold;
}


// Calculate the NDVI image from the original IRGB image 
std::vector<float> calculateNDVI(const std::vector<unsigned char>& image, const int Width, const int Height)
{
  int irchannel=0;
  int bluechannel=2;
  std::vector<float> ndvi_raw;
  ndvi_raw.resize(Width*Height);
  // Do the NDVI calculation
  for (int dy=0; dy<Height; dy++){ 
    for (int dx=0; dx<Width; dx++){
      float irpixel = (float) image[4* dy * Width + 4 * dx + irchannel];
      float bluepixel = (float) image[4 * dy * Width + 4 * dx + bluechannel];
      float numerator = (irpixel - bluepixel);
      float denominator = (irpixel + bluepixel);
      float pixel = (numerator / denominator);
      ndvi_raw[dy * Width + dx] = pixel;
    }
  }
  return ndvi_raw;
} 


// Calculate the minimum and maximum pixel values in an image
void minMax(const std::vector<float>& image, const int Width, const int Height, float& min, float& max)
{
  // Calculate the min and max values:  
  for (int dy=0; dy<Height; dy++){
    for (int dx=0; dx<Width; dx++){
      if(image[dy * Width + dx] < min) 
	min = image[dy * Width + dx];
      if(max < image[dy * Width + dx]) 
	max = image[dy * Width + dx];
    }
  }

}


// Scale a float image into the normal 0-255 greyscale range
std::vector<float> scaleImage(const std::vector<float>& image, const int Width, const int Height, const float min, const float max)
{
  double data_black = min;
  double data_white = max;
  double range = data_white - data_black;
  
  std::vector<float> scaled;
  scaled.resize(Width*Height);
  for (int dy=0; dy<Height; dy++){
    for (int dx=0; dx<Width; dx++){
      scaled[dy * Width + dx] = (float) (((image[dy * Width + dx] - data_black)/range) * 255);
    }
  }
  return scaled;
}


// Threshold a greyscale image
 std::vector<int> thresholdImage(const std::vector<float>& image, const int Width, const int Height, const int threshold)
{
  std::vector<int> bitmap;
  bitmap.resize(Width*Height); // make sure we have space
  for (int dy=0; dy<Height; dy++){
    for (int dx=0; dx<Width; dx++){
      if(image[dy * Width + dx] >= threshold)
	bitmap[dy * Width + dx] = 255;
      else
	bitmap[dy * Width + dx] = 0;
    }
  }
  return bitmap;
}


// Convert a greyscale (0-255) image to RGB
// Output will be 3 identical channels plus alpha in 4 byte RGBARGBA format
template<typename T>
std::vector<unsigned char> greyscale2RGB(const std::vector<T>& image, const int Width, const int Height)
{
  std::vector<unsigned char> output;
  output.resize(Width * Height * 4); // Output image will by 4x bigger than the input
  
  for (int dy=0; dy<Height; dy++){
    for (int dx=0; dx<Width; dx++){
      output[4 * Width * dy + 4 * dx + 0] = (unsigned char) image[dy * Width + dx];
      output[4 * Width * dy + 4 * dx + 1] =  (unsigned char) image[dy * Width + dx];
      output[4 * Width * dy + 4 * dx + 2] =  (unsigned char) image[dy * Width + dx];
      output[4 * Width * dy + 4 * dx + 3] =  (unsigned char) 255;
    }
  }
  return output;
}


// Reduce the NDVI into a single relative metric by summing over all vegetation pixels
float sumVegetationIndex(const std::vector<float>&ndvi_raw, const std::vector<int>& bitmap, const int Width, const int Height)
{
  float sumVegIndex = 0.0;
  for (int dy=0; dy<Height; dy++){
    for (int dx=0; dx<Width; dx++){
      if(bitmap[dy * Width + dx] == 255)
	sumVegIndex += ndvi_raw[dy * Width + dx];
    }
  }
  return sumVegIndex;
}


// Displays help message. 
static int help(void)
{
  fprintf(stderr, 
	  "Usage: planthealth [-h] [-d] [-b] [-o output.png] input.png\n"
          "\t-h Display this help message.\n"
          "\t-d Verbose output.\n"
          "\t-b Output the bitmap image to [output] instead of the NDVI.\n"
          "\t-o Output the Scaled NDVI image to [output].\n"
          "\t   Input and Output images must be PNG Format.\n"
          "Nick Arini 2014\n");
  exit(0);

}


int main(int argc, char **argv) {
  
  int optch;
  int outputFlag=0;
  int outputBitmap=0;
  char *b_opt_arg;

  // command line arguments
  while ((optch = getopt(argc, argv, ":dhbo:")) != EOF)
    switch (optch) {
    case 'd':
      debug = 1;
      break;
    case 'h':
      help();
      break;
    case 'b':
      outputBitmap=1;
      break;
    case 'o':
      outputFlag=1;
      b_opt_arg = optarg;
      break;
    case ':':
      help();
      break;
    case '?':
      help();
      break;
    default:
      help();
      break;
    }
  
  // check command line arguments
  if (argc - optind != 1) {
    help();
    exit(1);
   }

  // Grab the image from file
  //const char* filename = argc > 1 ? argv[1] : "image2.png";
  const char* filename =argv[optind];
  std::vector<unsigned char> image; //the raw pixels
  int Width=0, Height=0;
  loadPNG(filename, image, Width, Height);
  if(debug)
    printf("Filename %s loaded\n",filename);
  
  // Now calculate the NDVI Image
  std::vector<float> ndvi_raw = calculateNDVI(image, Width, Height);

  float min=0.0, max=0.0;
  minMax(ndvi_raw, Width, Height, min, max);

  if(debug){
    printf("NDVI Calculated:\n");
    printf("Min NDVI: %f\n", min);
    printf("Max NDVI: %f\n", max);
  }
  
  // Now we need to scale the image in our normal 0-255 range
  // Keep the raw image because we need it later
  std::vector<float> scaled = scaleImage(ndvi_raw, Width, Height, min, max);

  //now do the thresholding 
  int threshold = otsu_threshold(scaled, Width, Height);
  if(debug)
    printf("Calculating Otsu Threshold: %d \n", threshold );

  // Apply the threshold to the scaled image to create a bitmap which excludes non-vegetation
  std::vector<int> bitmap = thresholdImage(scaled, Width, Height, threshold);
  if(debug)
    printf("Thresholding Image\n");

  // Loop through the original NVDI Raw image checking against the bitmap and summing the vegetation index over all plant pixels.
  // The higher this value the more overall photosynthesis is going on with the plant.
  float totalVegIndex = sumVegetationIndex(ndvi_raw, bitmap, Width, Height);
  if(debug)
    printf ("Total Vegetation Index: %f\n", totalVegIndex);
  else
    printf("%f\n", totalVegIndex); // the main output which can be grabbed clean by a script


 // Optional save scaled NDVI (or bitmap) image to disk
  if(outputFlag){
    const char* filename2 = b_opt_arg; // The optional argument we captured above
    if(debug)
      printf("Filename %s\n", filename2);

    std::vector<unsigned char> outputimage;
    if(outputBitmap)
      outputimage = greyscale2RGB(bitmap, Width, Height);
    else
      outputimage = greyscale2RGB(scaled, Width, Height);
    
    if(debug)
      printf("Encoding PNG Image %s\n", filename2);
    
    savePNG(filename2, outputimage, Width, Height);

    if(debug)
      printf("%s Saved\n", filename2);
  }


  if(debug)
    printf("Done!\n");
  
  return 0;
  
}
