#!/bin/bash

# Use this script to list all the files in all subdirectories sorted from the biggest to the smallest.
# Since list can be rather long its best to pipe it into file: > list.txt

find . -type f \
  ! -path "*/.*/*" \
  ! -path "*/Saved/*" \
  ! -path "*/Binaries/*" \
  ! -path "*/Build/*" \
  ! -path "*/DerivedDataCache/*" \
  ! -path "*/Intermediate/*" \
  ! -path "*/Platforms/*" \
  ! -path "*/__*__/*" \
  -exec du -ah {} + | sort -rh