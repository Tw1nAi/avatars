# Define the path to the main directory
$mainDir = "C:\Juice\Awatary\Audio_Dialogi"

# Define the path to the output file
$outputFile = "C:\Juice\Awatary\Audio_Dialogi\output.txt"

# Check if the output file already exists and delete it to start fresh
if (Test-Path $outputFile) {
  Remove-Item $outputFile
}

# Iterate through each child directory in the main directory (character directories)
Get-ChildItem -Path $mainDir -Directory | ForEach-Object {
  $characterName = $_.Name

  # Write the character name to the output file
  Add-Content -Path $outputFile -Value "Character name: $characterName"

  # Iterate through each child directory of the character directory (language directories)
  Get-ChildItem -Path $_.FullName -Directory | ForEach-Object {
      $languageName = $_.Name
      $audioDir = Join-Path $_.FullName "Audio"

      # Write the language name to the output file
      Add-Content -Path $outputFile -Value "`tLanguage: $languageName"

      # Check if the Audio directory exists
      if (Test-Path $audioDir) {
          # List all .wav files in the Audio directory
          $audioFiles = Get-ChildItem -Path $audioDir -Filter *.wav

          # Iterate through each audio file and write its name to the output file, indented
          foreach ($file in $audioFiles) {
              Add-Content -Path $outputFile -Value "`t`t$file"
          }
      }

      # Add a newline for readability between languages
      Add-Content -Path $outputFile -Value ""
  }

  # Add an additional newline for readability between characters
  Add-Content -Path $outputFile -Value ""
}

Write-Host "Output file has been created at $outputFile"