#! /bin/bash

# Invalid method argument should return < 0 

../src/mm 1000 invalidmethod 2>&1 | grep Unsupported > /dev/null
