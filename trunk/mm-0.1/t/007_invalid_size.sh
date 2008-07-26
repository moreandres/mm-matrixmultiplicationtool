#! /bin/bash

# Invalid size should be reported

../src/mm -20 simple 2>&1 | grep invalid > /dev/null
