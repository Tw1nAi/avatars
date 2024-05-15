# pass the file list and destination directory as arguments

# Specify the destination directory where you want to copy the files
$destinationDirectory = "c:\Juice\Awatary\Audio_Dialogi\JanZumbach\EN\Audio\missing"

# Check if the destination directory exists, if not, create it
if (-not (Test-Path -Path $destinationDirectory)) {
    New-Item -ItemType Directory -Path $destinationDirectory | Out-Null
}

# Read the list of files from "files.txt"
$fileList = Get-Content "files.txt" -ErrorAction Stop

# Loop through each file in the list and copy it to the destination directory
foreach ($file in $fileList) {
    # Get the file name from the full path
    $fileName = [System.IO.Path]::GetFileName($file)

    # Create the full destination path by combining the destination directory and the file name
    $destinationPath = Join-Path -Path $destinationDirectory -ChildPath $fileName

    # Copy the file to the destination directory
    Copy-Item -Path $file -Destination $destinationPath -Force
}

Write-Host "Files copied successfully to $destinationDirectory"