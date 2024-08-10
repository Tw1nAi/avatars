#!/bin/bash

# Default directory paths
dirA="/path/to/default/dirA"
dirB="/path/to/default/dirB"
dirC="/path/to/default/dirC"

# Function to display usage instructions
usage() {
    echo "Usage: $0 [-a dirA] [-b dirB] [-c dirC]"
    echo "  -a | --directoryA      : Directory A (default: $dirA)"
    echo "  -b | --directoryB      : Directory B (default: $dirB)"
    echo "  -c | --directoryC      : Directory C (default: $dirC)"
    exit 1
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -a|--directoryA) dirA="$2"; shift ;;
        -b|--directoryB) dirB="$2"; shift ;;
        -c|--directoryC) dirC="$2"; shift ;;
        -h|--help) usage ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done

# Ensure Directory C exists
if [ ! -d "$dirC" ]; then
    mkdir -p "$dirC"
fi

# Get all files from both directories, without extensions
filesInA=$(find "$dirA" -type f -exec basename {} \; | sed 's/\.[^.]*$//')
filesInB=$(find "$dirB" -type f -exec basename {} \; | sed 's/\.[^.]*$//')

# Find files in A that are not in B
diffFiles=$(comm -23 <(echo "$filesInA" | sort) <(echo "$filesInB" | sort))

# Copy the files to Directory C
for name in $diffFiles; do
    originalFiles=$(find "$dirA" -type f -name "$name.*")

    for file in $originalFiles; do
        targetPath="$dirC/$(basename "$file")"
        if [ ! -f "$targetPath" ]; then
            cp "$file" "$targetPath"
            echo "Copied $file to $targetPath"
        else
            echo "Skipping, target already exists: $targetPath"
        fi
    done
done

# Optional: Output the list of files not in B to a text file
# echo "$diffFiles" > ./files.txt