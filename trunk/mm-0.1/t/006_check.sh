#! /bin/bash

# Double check simple algorithm

../src/mm 100 simple --check | grep -v Failed > /dev/null
