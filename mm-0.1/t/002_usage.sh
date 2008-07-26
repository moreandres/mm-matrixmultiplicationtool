#! /bin/bash

# Execution without arguments must show proper message

../src/mm 2>&1 | grep Usage > /dev/null
