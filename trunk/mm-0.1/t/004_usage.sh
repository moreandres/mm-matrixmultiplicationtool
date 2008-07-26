#! /bin/bash

# Execution using --usage must show proper message

../src/mm --usage 2>&1 | grep Usage > /dev/null