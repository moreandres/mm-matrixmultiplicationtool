#! /bin/bash

# Double check transp algorithm

../src/mm 100 transp --check | grep -v Failed > /dev/null
