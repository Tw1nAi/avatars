from setuptools import find_packages, setup

from whisper_live.__version__ import __version__

setup(name="Transcription Server",
      version=__version__,
      description="OpenAI's Whisper adapted for live transcription from mic. Based on WhisperLive by https://github.com/collabora.",
      long_description_content_type="text/markdown",
      include_package_data=True,
      author="Mateusz Fryc",
      author_email="mateuszfryc@gmail.com",
      classifiers=[
          "Development Status :: Alpha",
          "Intended Audience :: Developers",
          "Intended Audience :: Science/Research",
          "License :: OSI Approved :: MIT License",
          "Programming Language :: Python :: 3",
          "Programming Language :: Python :: 3 :: Only",
          "Programming Language :: Python :: 3.8",
          "Programming Language :: Python :: 3.9",
          "Topic :: Scientific/Engineering :: Artificial Intelligence",
      ],
      packages=find_packages(
        exclude=("examples",
                 "Audio-Transcription-Chrome",
                 "Audio-Transcription-Firefox",
                 "requirements",
                 "whisper-finetuning"
                )
      ),
      install_requires=[
        "PyAudio",
        "faster-whisper==0.10.0",
        "torch",
        "torchaudio",
        "websockets",
        "onnxruntime",
        "ffmpeg-python",
        "scipy",
        "websocket-client",
      ],
      python_requires=">=3.8"
)
