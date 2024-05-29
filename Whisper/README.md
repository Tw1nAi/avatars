# Whisper live transcription with GPU support

This code is an attempt to provide very simple live transcription of streamed audio via websockets and Silero VAD to filter out silent audio samples. It's based on modified code of Whisper Live: [https://github.com/collabora/WhisperLive](https://github.com/collabora/WhisperLive) based on Faster Whisper [https://github.com/SYSTRAN/faster-whisper](https://github.com/SYSTRAN/faster-whisper)

# How it works?

There are two independed python scripts at play:

- server: it receives audio stream via websockets from single `audio_stream_client` and runs continous transcription that is being sent with websockets to single `text_consumer_client`.
- microphone streaming client: it continously tries to connect to server and once connected streams audio in float32 array from operating system's default audio input device

# Installation

Create environment:

`conda create -p ./.env python=3.10`

activate it:

`conda activate ./.env`

Make sure all packages listed in requirements.txt are avilable in your python environment.
`pip install -r ./requirements.txt`

In case server throws error about missing websockets client try:
`python -m pip install websockets`

In case the server.py would quit with error message `ValueError: Invalid CPU device index: 1` install additional torch and cuda support:
`pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118`

# How to use it?

1. Run server locally or from docker. Make sure to set the language in TranscriptionServer initial parameters.
2. Provide server with audio stream either by running `client.py` script or by streaming audio in float32 array format to the server.
3. Use your websocket client to receive transcription from server.

### Local server

Just run the `server.py` script in your local python environment, e.g.:

```
  python server.py
```

### Server in docker

Docker image is based on Nvidia's official image that supports CUDA.

```
  docker build . -t custom-whisper -f dockerfile
  docker run -itd --name custom-whisper --gpus all -p 9090:9090 custom-whisper:latest
```

Once docker container is running you can watch its logs either in Docker Desktop GUI or by running:

```
docker logs -f custom-docker
```

# Known issues

In case server would not run with GPU (which is favored way, since its way faster) reinstall your torch to the latest version.
