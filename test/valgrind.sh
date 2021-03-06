#!/bin/sh

if command -v valgrind; then
    for file in hash buffer list vector string map
    do
        echo [$file]
        if ! valgrind  --error-exitcode=1 --read-var-info=yes --leak-check=full --show-leak-kinds=all test/$file; then
            exit 1
        fi
    done
fi
