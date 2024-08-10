#!/bin/bash

# Default values
directory="/path/to/default/directory"
stringToChange="_bsweight"
replacement="animFace_"

# Function to display usage
usage() {
    echo "Usage: $0 [-d directory] [-s string_to_change] [-r replacement_string]"
    echo "  -d | --directory       : Directory to search files in (default: $directory)"
    echo "  -s | --string          : String to change in file names (default: $stringToChange)"
    echo "  -r | --replacement     : Replacement string (required)"
    exit 1
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -d|--directory) directory="$2"; shift ;;
        -s|--string) stringToChange="$2"; shift ;;
        -r|--replacement) replacement="$2"; shift ;;
        -h|--help) usage ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done

# Ensure replacement string is provided
if [[ -z "$replacement" ]]; then
    echo "Error: Replacement string is required."
    usage
fi

# Function to replace string in file names
replace_string_in_filenames() {
    for file in "$directory"/*; do
        if [[ -f "$file" && "$file" == *"$stringToChange"* ]]; then
            newName="${file//$stringToChange/$replacement}"
            mv "$file" "$newName"
            echo "File '$file' renamed to '$newName'"
        fi
    done
}

# Call the function
replace_string_in_filenames