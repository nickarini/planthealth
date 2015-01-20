import datetime
import time
import picamera
import subprocess
import os, errno

outputdir = '/home/pi/'  # outputdir must exist and be writable
outputstats = 'ndvi_stats.csv'

f = open(outputstats,'w')


var = 1 	
while var == 1 :  # This constructs an infinite loop
    
    st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d_%H-%M-%S')
    print st
    
    with picamera.PiCamera() as camera:
        camera.start_preview()
        time.sleep(1)
        filename = '%s.png' % st
        camera.capture(outputdir+filename)
        camera.stop_preview()


    index = subprocess.check_output('../c++/src/planthealth %s' % outputdir+filename, shell=True)
    f.write(index)
    print index 

    # now delete the file so we dont fill up the disk (need to make this an optional
    try:
        os.remove(outputdir+filename)
    except OSError:
        pass

    time.sleep(60)



