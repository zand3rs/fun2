#!/bin/bash
javac -classpath ".:http_client.jar:db.jar:servlet-api.jar:commons-httpclient-3.1.jar:commons-logging-1.1.1.jar:log4j.jar:commons-dbcp-1.2.2.jar:commons-pool-1.4.jar:spiffy-log4j.jar" com/yuicon/utils/db/SimpleDBConnection.java
javac -classpath ".:http_client.jar:db.jar:servlet-api.jar:commons-httpclient-3.1.jar:commons-logging-1.1.1.jar:log4j.jar:commons-dbcp-1.2.2.jar:commons-pool-1.4.jar:spiffy-log4j.jar:http_client.jar" com/yuicon/fun2/ards/ARDSResponse.java
jar -cvf fun2.jar com/yuicon/fun2/ards/*.class com/yuicon//utils/db/*.class
