#!/bin/sh

xmin=101220
ymin=37699
xmax=101232
ymax=37720
zoom=$1

xmin=`awk -v z=$zoom -v xmin=$xmin 'BEGIN{for(i=19;i>z;i--){xmin=int(xmin/2)};print xmin}'`
ymin=`awk -v z=$zoom -v ymin=$ymin 'BEGIN{for(i=19;i>z;i--){ymin=int(ymin/2)};print ymin}'`
xmax=`awk -v z=$zoom -v xmax=$xmax 'BEGIN{for(i=19;i>z;i--){xmax=int(xmax/2)};print xmax}'`
ymax=`awk -v z=$zoom -v ymax=$ymax 'BEGIN{for(i=19;i>z;i--){ymax=int(ymax/2)};print ymax}'`

for i in  `seq $xmin $xmax`
do
	for j in `seq $ymin $ymax`
	do
		wget  'http://online1.map.bdimg.com/onlinelabel/?qt=tile&x='$i'&y='$j'&z='$zoom'&styles=pl&udt=20170712&scaler=1&p=1' -O ${i}_${j}_${zoom}.png
	done
done
python test.py $xmin $xmax $ymin $ymax $zoom

