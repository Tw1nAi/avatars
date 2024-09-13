import threading
import time
from typing import Callable, List, Optional

import numpy as np
import torch
from custom_logger import info
from faster_whisper import WhisperModel


class TranscriptionService:
    def __init__(
        self,
        language="pl",
        model_size="large-v2",
        device_index=0,
        rate=16000,
        debug=False,
        excluded_phrases: Optional[List[str]] = None,
        min_sample_duration: float = 0.5,  # Minimum duration in seconds for processing
    ):
        # Configuration
        self.language = language
        self.model_size = model_size
        self.device_index = device_index
        self.rate = rate
        self.debug = debug
        self.excluded_phrases = excluded_phrases if excluded_phrases else []
        self.min_sample_duration = min_sample_duration

        # Device setup
        self.device = "cuda" if torch.cuda.is_available() else "cpu"

        # Initialize the Whisper model
        self.model = WhisperModel(
            model_size,
            device_index=device_index,
            device=self.device,
            compute_type="float16" if self.device == "cuda" else "int8",
        )

        # Transcription state
        self.audio_buffer = np.array([], dtype=np.float32)
        self.buffer_lock = threading.Lock()
        self.exit_event = threading.Event()
        self.on_transcription_callback: Optional[Callable[[str], None]] = None  # Event handler for transcription changes

        # Start the transcription thread
        self.transcription_thread = threading.Thread(target=self._transcription_loop, daemon=True)
        self.transcription_thread.start()

        if self.debug:
            print(f"TranscriptionService initialized with device: {self.device}, device_index: {self.device_index}")

    def add_frames(self, frames: np.ndarray):
        """Add audio frames to the transcription buffer."""
        with self.buffer_lock:
            self.audio_buffer = np.concatenate((self.audio_buffer, frames))
            if self.debug:
                print(f"Added frames: {len(frames)}, total buffer size: {len(self.audio_buffer)}")

    def set_on_transcription_callback(self, callback: Callable[[str], None]):
        """Set the event handler for transcription changes."""
        self.on_transcription_callback = callback

    def _transcription_loop(self):
        """Internal transcription loop running in a separate thread."""
        while not self.exit_event.is_set():
            with self.buffer_lock:
                buffer_length = len(self.audio_buffer)
            if buffer_length == 0:
                # No data but may receive more
                time.sleep(0.01)
                continue
            else:
                with self.buffer_lock:
                    # Copy the buffer for processing
                    audio_data = self.audio_buffer.copy()

            duration = len(audio_data) / self.rate
            if duration < self.min_sample_duration:
                # Not enough data to process
                time.sleep(0.05)
                continue

            try:
                if self.debug:
                    print(f"Processing audio data of duration: {duration:.2f} seconds")

                segments_generator, info = self.model.transcribe(
                    audio_data,
                    language=self.language,
                    vad_filter=True
                )
                segments = list(segments_generator)

                # Process the transcription result
                if segments:
                    # Extract text from segments
                    text = ' '.join([seg.text.strip() for seg in segments])

                    # Exclude phrases if needed
                    if not self._includes_excluded_phrase(text):
                        # Call the transcription event handler
                        if self.on_transcription_callback:
                            self.on_transcription_callback(text)
                    else:
                        if self.debug:
                            print(f"Excluded phrase detected in text: {text}")
                else:
                    if self.debug:
                        print("No segments returned from transcription.")

                # Remove processed data from buffer
                processed_samples = len(audio_data)
                with self.buffer_lock:
                    self.audio_buffer = self.audio_buffer[processed_samples:]
                    if self.debug:
                        print(f"Processed {processed_samples} samples, remaining buffer size: {len(self.audio_buffer)}")
            except Exception as e:
                # Handle exception
                if self.debug:
                    print(f"Transcription error: {e}")
                time.sleep(0.1)

    def _includes_excluded_phrase(self, text: str) -> bool:
        """Check if the text includes any excluded phrases."""
        text_lower = text.strip().lower()
        for phrase in self.excluded_phrases:
            if phrase.strip().lower() in text_lower:
                return True
        return False

    def stop(self):
        """Stop the transcription service."""
        self.exit_event.set()
        self.transcription_thread.join()
        if self.debug:
            print("TranscriptionService stopped.")

    def get_status_message(self):
        return f"Model: {self.model_size}. Device: {self.device}. Device index: {self.device_index}."

    def set_language(self, language):
        if language == self.language: return

        self.language = language
        info(f"Language set to: {language}")