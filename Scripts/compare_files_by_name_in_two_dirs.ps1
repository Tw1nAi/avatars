# Define the paths to directories A and B
$dirA = "c:\Juice\Awatary\Audio_Dialogi\JanKowalewski\EN\Audio"
$dirB = "C:\Juice\Awatary\Audio_Dialogi\JanKowalewski\EN\Animations"
$dirC = "c:\Juice\Awatary\Audio_Dialogi\JanKowalewski\EN\Audio_Missing"

# Ensure Directory C exists
if (-not (Test-Path -Path $dirC)) {
  New-Item -Path $dirC -ItemType Directory
}

# Get all files from both directories, without extensions
$filesInA = Get-ChildItem -Path $dirA -Recurse -File | ForEach-Object { $_.BaseName }
$filesInB = Get-ChildItem -Path $dirB -Recurse -File | ForEach-Object { $_.BaseName }

# Find files in A that are not in B
$diffFiles = $filesInA | Where-Object { $_ -notin $filesInB }

# Copy the files to Directory C
$diffFiles | ForEach-Object {
  $name = $_
  $originalFiles = Get-ChildItem -Path $dirA -Recurse -File | Where-Object { $name -eq $_.BaseName }

  foreach ($file in $originalFiles) {
      $targetPath = Join-Path -Path $dirC -ChildPath $file.Name
      if (-not (Test-Path -Path $targetPath)) {
          Copy-Item -Path $file.FullName -Destination $targetPath
      } else {
          Write-Host "Skipping, target already exists: $targetPath"
      }
  }
}

# Output the list of files not in B to a text file
# $diffFiles | Out-File -FilePath ".\files.txt"