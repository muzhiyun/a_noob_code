import os
import time
import random
stime=time.time()
for pathname,dirnames,filenames in os.walk('./pythonimages'):
     for filename in filenames:
	if (random.randint(0,9))>=5:
	   file=os.path.join(pathname,filename)
           os.remove(file)
ftime=time.time()
print ftime-stime
