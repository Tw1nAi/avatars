# Function to replace string in file names
function Replace-StringInFileNames {
  param (
      [string]$directory,
      [string]$stringToChange,
      [string]$replacement
  )

  # Get all files in the specified directory
  $files = Get-ChildItem -Path $directory -File

  # Loop through each file
  foreach ($file in $files) {
      # Check if the file name contains the string to change
      if ($file.Name -like "*$stringToChange*") {
          # Replace the string in the file name with the replacement string
          $newName = $file.Name -replace $stringToChange, $replacement

          # Rename the file
          Rename-Item -Path $file.FullName -NewName $newName -Force

          Write-Host "File '$($file.Name)' renamed to '$newName'"
      }
  }
}

# Usage example
$directory = "C:\Users\squid\Juice\Dialogi\WojtekTheBear\Fixes\Anim"
$stringToChange = "_bsweight"
$replacement = "animFace"

Replace-StringInFileNames -directory $directory -stringToChange $stringToChange -replacement $replacement
