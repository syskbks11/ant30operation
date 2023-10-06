#!/bin/sh

for file in *.cpp
do
	echo $file
	cp $file ${file}~
	sed -e 's/"..\/..\/import\/libtkb.h"/"..\/libtkb\/export\/libtkb.h"/g' ${file}~ > ${file}
	diff ${file}~ ${file}
done

#end
