Planthealth - Raspberry Pi Camera Automation
============

### Description: 

Script to capture plant images automatically at regular intervals and run the planthealth analysis

Language:    Python

Date:        18th December 2014

Author:      Nick Arini

### References:  

http://www.raspberrypi.org/products/pi-noir-camera/
http://www.raspberrypi.org/learning/python-picamera-setup/

### Usage: 

This script assumes you are using a Raspberry Pi NoIR Camera with the supplied blue filter

Install and enable the camera as described here: http://www.raspberrypi.org/learning/python-picamera-setup/

Make sure the python pycamera module is installed on your pi

```sudo apt-get install python-picamera python3-picamera```
 
Make sure the output directory exists and is writable

```
outputdir = '/home/pi'  # outputdir must exist and be writable
outputstats = 'ndvi_stats.csv'
```

Now simply run the script:

```python ./camscript.py```

This will write the raw images to the output dir with the filenames as the datetime of capture

Stats will be written to the outputstats.csv file. 





