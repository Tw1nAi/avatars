#!/bin/bash

# Default values for main directory and output file
mainDir="/path/to/default/mainDir"
outputFile="/path/to/default/output.txt"

# Function to display usage instructions
usage() {
    echo "Usage: $0 [-d main_directory] [-o output_file]"
    echo "  -d | --directory      : Main directory to search (default: $mainDir)"
    echo "  -o | --output         : Output file path (default: $outputFile)"
    exit 1
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -d|--directory) mainDir="$2"; shift ;;
        -o|--output) outputFile="$2"; shift ;;
        -h|--help) usage ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done

# Check if the output file already exists and delete it to start fresh
if [ -f "$outputFile" ]; then
    rm "$outputFile"
fi

# Iterate through each child directory in the main directory (character directories)
for characterDir in "$mainDir"/*/; do
    characterName=$(basename "$characterDir")

    # Write the character name to the output file
    echo "Character name: $characterName" >> "$outputFile"

    # Iterate through each child directory of the character directory (language directories)
    for languageDir in "$characterDir"/*/; do
        languageName=$(basename "$languageDir")
        audioDir="$languageDir/Audio"

        # Write the language name to the output file
        echo -e "\tLanguage: $languageName" >> "$outputFile"

        # Check if the Audio directory exists
        if [ -d "$audioDir" ]; then
            # List all .wav files in the Audio directory
            audioFiles=("$audioDir"/*.wav)

            # Iterate through each audio file and write its name to the output file, indented
            for file in "${audioFiles[@]}"; do
                if [ -f "$file" ]; then
                    echo -e "\t\t$(basename "$file")" >> "$outputFile"
                fi
            done
        fi

        # Add a newline for readability between languages
        echo "" >> "$outputFile"
    done

    # Add an additional newline for readability between characters
    echo "" >> "$outputFile"
done

echo "Output file has been created at $outputFile"