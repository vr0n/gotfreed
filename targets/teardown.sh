#!/bin/sh

find ./ -type f | grep -E 'example_[0-9]$' | xargs rm
