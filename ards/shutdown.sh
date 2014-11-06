#!/bin/bash
pid=`ps -ef |grep ARDSResponse.cfg|grep java|grep ARDSResponse|egrep -v "grep"`
if [ $pid > 0 ] ; then
	kill $pid
	echo "DONE Killing ARDSResponse Daemon";
else
	echo "ARDSResponse Daemon Not Running";
fi
