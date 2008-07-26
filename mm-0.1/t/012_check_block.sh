#! /bin/bash

# Double check block algorithm

../src/mm 100 block --check 2>&1 | grep -v Failed > /dev/null
