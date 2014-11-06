#!/bin/bash
CompressDay=`date --date='2 days ago' +%F|awk -F- '{print $1$2$3}'`;
RetentionDay=`date --date='3 days ago' +%F|awk -F- '{print $1$2$3}'`;
today=`date +%F`;
file=/tmp/list;
ls -lt /appl/fun2/log/*.txt > $file;
list=`cat $file|grep -v $today| awk '{print $9}'`;
echo "FUN2 ARDS LOGROTATION STARTING";	
for node in $list 
do 
	tempdate=`echo $node|awk -F. '{print $2}'|awk -F- '{print $1$2$3}'`;
	echo "This is the tempDate $tempdate";	
	if [ "$tempdate" -eq "$CompressDay" ]; then
		echo "Archiving $node";
		gzip $node;
	fi
	if [ "$CompressDay" -gt "$tempdate" ]; then
		echo "Archiving $node";
		gzip $node;
	else
		echo "Skipping $node for archival";
	fi 
#	if [ "$tempdate" < "$RetentionDay" ]; then
#		echo "$node is for deletion."
#		rm $file
#	fi 
done 
echo "FUN2 ARDS LOGROTATION FINISHED";	
