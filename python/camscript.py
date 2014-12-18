import datetime
import time
import picamera
import subprocess

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


    index = subprocess.check_output('../planthealth %s' % filename, shell=True)
    f.write(index)
    print index 

    time.sleep(60)



