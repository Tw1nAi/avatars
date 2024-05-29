start cmd.exe /k docker run -it --name custom-whisper --gpus all -p 9090:9090 custom-whisper:latest

rem docker ps -q -f name=custom-whisper > temp.txt

rem SET /p CONTAINER_ID = < temp.txt
rem IF "%CONTAINER_ID%" == "" (
rem     echo Starting container...
rem     start cmd.exe /k docker run -it --name custom-whisper --gpus all -p 9090:9090 custom-whisper:latest
rem     del temp.txt
rem ) ELSE (
rem     echo Container is already running.
rem     del temp.txt
rem     exit
rem )