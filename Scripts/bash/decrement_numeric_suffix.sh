#!/bin/bash

# Default directory is the current working directory
directory=$(pwd)

# Function to display usage instructions
usage() {
    echo "Usage: $0 [-d directory]"
    echo "  -d | --directory      : Directory to search (default: current directory)"
    exit 1
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -d|--directory) directory="$2"; shift ;;
        -h|--help) usage ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done

# Check if the directory exists
if [ ! -d "$directory" ]; then
    echo "Error: Directory '$directory' does not exist."
    exit 1
fi

# Iterate through each file in the specified directory
for file in "$directory"/*; do
    if [[ -f "$file" ]]; then
        fileName=$(basename "$file")
        fileExtension="${fileName##*.}"
        baseName="${fileName%-*}"
        suffix="${fileName##*-}"

        # Check if the suffix is numeric and has exactly two digits
        if [[ "$suffix" =~ ^[0-9]{2}$ ]]; then
            # Decrement the numeric suffix
            newSuffix=$(printf "%02d" $((10#$suffix - 1)))

            # Construct the new file name
            newFileName="$baseName-$newSuffix.$fileExtension"

            # Rename the file
            mv "$file" "$directory/$newFileName"
        fi
    fi
done
