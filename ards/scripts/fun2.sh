#!/bin/bash

BASE_DIR="${0%/*}"
APP_BIN_DIR=""
APP_NAME="FUN2"
##---------------------------------------------------------------------------

##---------------------------------------------------------------------------

case $1 in
    start)
        echo "Starting $APP_NAME..."
	cd /appl/fun2/tomcat/bin/
	./startup.sh
	cd /appl/fun2/ARDSResponse
	./ARDSResponse.sh
        echo "Done."
        ;;
    stop)
        echo "Stopping $APP_NAME..."
	cd /appl/fun2/tomcat/bin/
	./shutdown.sh
	TOMCATPID=`ps -ef |grep java |grep tomcat|awk '{print $2}'`
	if [ -z "$TOMCATPID" ]; then
		echo "TOMCAT stopped"
	else
		echo "TOMCAT is still alive, try: $0 kill"
	fi
	ARDSPID=`ps -ef |grep java |grep ARDSResponse.cfg|awk '{print $2}'`
	kill $ARDSPID
	ARDSPID=`ps -ef |grep java |grep ARDSResponse.cfg|awk '{print $2}'`
	if [ -z "$ARDSPID" ]; then
		echo "ARDSRESPONSE stopped"
	else
		echo "ARDSRESPONSE is still alive, try: $0 kill"
	fi
        echo "Done."
        ;;
    kill)
        echo "Killing $APP_NAME..."
        for APP_BIN in $APP_BIN_LIST; do
            pid=`pgrep -f $APP_BIN`
            if [ ! -z "$pid" ]; then
                kill -9 $pid
            fi
            echo "$APP_BIN killed"
        done
        echo "Done."
        ;;
    status)
	TOMCAT=`ps -ef |grep java|grep tomcat| egrep -v 'grep'|wc -l`
	ARDS=`ps -ef |grep java|grep ARDS| egrep -v 'grep'|wc -l`
	#APACHE=`ps -ef |grep '/usr/local/apache/bin/httpd'| egrep -v 'grep'|egrep 'root'|wc -l`
	APACHE=`ps -ef |grep 'httpd'| egrep -v 'grep'|egrep 'root'|wc -l`

	if [ $TOMCAT -eq 1 ]; then
		echo "Fun2 TOMCAT is alive";
	else
		echo "Fun2 TOMCAT is off";
	fi
	if [ $ARDS -eq 1 ]; then
		echo "Fun2 ARDSResponse is alive";
	else
		echo "Fun2 ARDSResponse is off";
	fi
	if [ $APACHE -eq 1 ]; then
		echo "Fun2 APACHE WEBTOOL is alive";
	else
		echo "Fun2 APACHE WEBTOOL is off";
	fi
        ;;
    logrotate)
        echo "$APP_NAME: Rotating logs..."
        for APP_BIN in $APP_BIN_LIST; do
            pid=`pgrep -f $APP_BIN`
            if [ ! -z "$pid" ]; then
                kill -HUP $pid
            fi
            echo "$APP_BIN"
        done
        echo "Done."
        ;;
    *)
        echo "usage: $0 [start|stop|kill|status|logrotate]"
        ;;
esac

