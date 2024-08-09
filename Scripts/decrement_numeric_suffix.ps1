# Get the current working directory
$path = Get-Location

# Get all files in the current directory
Get-ChildItem -Path $path -File | ForEach-Object {
    # Extract the current file name and extension
    $fileName = $_.BaseName
    $fileExtension = $_.Extension

    # Use regex to match the pattern with the suffix (e.g., -01) at the end of the file name
    if ($fileName -match '^(?<baseName>.+)-(?<suffix>\d{2})$') {
        $baseName = $matches['baseName']
        $suffix = $matches['suffix']

        # Decrement the numeric suffix
        $newSuffix = [int]$suffix - 1
        $newSuffix = $newSuffix.ToString("D2")

        # Construct the new file name
        $newFileName = "$baseName-$newSuffix$fileExtension"

        # Rename the file
        Rename-Item -Path $_.FullName -NewName $newFileName
    }
}