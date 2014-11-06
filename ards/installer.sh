#!/bin/bash
echo "INSTALLING FUN2 CPK"
mkdir /appl/fun2
cp -Rf scripts /appl/fun2/.
cp -Rf ARDSResponse /appl/fun2/.
cp jdk1.6.0_18.tar.gz /appl/fun2/.
cp tomcat.tar.gz /appl/fun2/.
ln -s /appl/log /appl/fun2/log
tar -zxvf jdk1.6.0_18.tar.gz
tar -zxvf tomcat.tar.gz 
mv jdk1.6.0_18 /appl/fun2/.
mv jakarta-tomcat-5.0.28 /appl/fun2/. 
ln -s /appl/fun2/jdk1.6.0_18 /appl/fun2/java 
ln -s /appl/fun2/jakarta-tomcat-5.0.28 /appl/fun2/tomcat
cd /appl/fun2/jdk1.6.0_18/bin
chmod ugo+wrx *
ln -s /appl/log /appl/fun2/tomcat/logs
cd /appl/fun2/tomcat/bin
chmod ugo+wrx *
cd /appl/fun2/scripts
chmod ugo+x *.sh
cd /appl/fun2/ARDSResponse
chmod ugo+x *.sh
echo "INSTALLING FUN2 CPK FINISHED"
