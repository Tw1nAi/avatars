#!/bin/bash

# List files in all subdirectories that are bigger then 100 MB. The list will sorted from the biggest to the smallest file.
# Since the list can be long its best to pipe it into file: > list.txt

find . -type f -size +99M -exec du -h {} + | sort -rh