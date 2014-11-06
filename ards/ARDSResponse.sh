#!/bin/bash
nohup java -classpath "fun2.jar:commons-dbcp-1.2.2.jar:ojdbc14.jar:commons-httpclient-3.1.jar:commons-codec-1.3.jar:commons-logging-1.1.1.jar:log4j.jar:spiffy-log4j.jar:commons-pool-1.4.jar:http_client.jar" com.yuicon.fun2.ards.ARDSResponse ../cfg/ARDSResponse.cfg & 
