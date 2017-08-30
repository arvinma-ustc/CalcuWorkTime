import PIL.Image as Image
import os, sys
from sys import argv

mw = 256 
 
xmin=int(argv[1])
xmax=int(argv[2])
ymin=int(argv[3])
ymax=int(argv[4])
zoom=int(argv[5])

toImage = Image.new('RGBA', (mw*(xmax-xmin+1), mw*(ymax-ymin+1)))

for y in range(ymin, ymax): 
    for x in range(xmin, xmax):

       
        fname = str(x)+"_"+str(y)+"_"+str(zoom)+".png"
	print fname
        fromImage = Image.open(fname)

        toImage.paste(fromImage, ((x-xmin) * mw, (ymax-y) * mw))

toImage.save('./'+str(zoom)+'.png')

