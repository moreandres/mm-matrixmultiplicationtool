#! /bin/bash

# Double check transp algorithm

../src/mm 100 cblas --check | grep -v Failed > /dev/null
