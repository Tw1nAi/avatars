#!/bin/bash

# Default values
fileList="files.txt"
destinationDirectory="/path/to/default/destination"

# Function to display usage instructions
usage() {
    echo "Usage: $0 [-f file_list] [-d destination_directory]"
    echo "  -f | --filelist           : Path to the file containing the list of files to copy (default: $fileList)"
    echo "  -d | --destination        : Destination directory to copy the files to (default: $destinationDirectory)"
    exit 1
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -f|--filelist) fileList="$2"; shift ;;
        -d|--destination) destinationDirectory="$2"; shift ;;
        -h|--help) usage ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done

# Check if the destination directory exists, if not, create it
if [ ! -d "$destinationDirectory" ]; then
    mkdir -p "$destinationDirectory"
fi

# Check if the file list exists
if [ ! -f "$fileList" ]; then
    echo "Error: File list '$fileList' not found."
    exit 1
fi

# Loop through each file in the list and copy it to the destination directory
while IFS= read -r file; do
    # Get the file name from the full path
    fileName=$(basename "$file")

    # Create the full destination path by combining the destination directory and the file name
    destinationPath="$destinationDirectory/$fileName"

    # Copy the file to the destination directory
    if [ -f "$file" ]; then
        cp "$file" "$destinationPath"
        echo "Copied $file to $destinationPath"
    else
        echo "File not found: $file"
    fi
done < "$fileList"

echo "Files copied successfully to $destinationDirectory"
