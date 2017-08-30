import PIL.Image as Image
import os, sys

mw = 256 
 



fpre = "x" 
toImage = Image.new('RGBA', (mw*12, mw*21))

for y in range(37700, 37720): 
    for x in range(101220, 101231):

       
        fname = str(x)+"_"+str(y)+"_19.png"
	print fname
        fromImage = Image.open(fname)

        toImage.paste(fromImage, ((x-101220) * mw, (37720-y) * mw))

toImage.save('/mnt/h/map/test.png')

