#!/bin/bash
nodemon -e .cpp -w main.cpp --exec "g++ main.cpp -o main && ./main || exit 0"
