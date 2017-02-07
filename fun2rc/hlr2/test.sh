#!/bin/bash
g++ test.cpp httpclient.cpp ../../libfuc/lib/libfuc.a -o test -lcurl -I../../libfuc/include -lrt
./test
