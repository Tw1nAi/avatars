import os
import re as regex
import threading
import time
from enum import Enum
from typing import List, Optional

import numpy
# import pyaudio
import torch
from custom_logger import error, info, warn
from transcriber import WhisperModel

# logger = logging.getLogger()
# logger.setLevel(logging.DEBUG)

class TranscriptEvent(Enum):
    # fired when silence is detected after speach in specyfied time
    SILENCE_AFTER = 0
    # fired when change in the transcription is detected
    TRANSCRIPTION_CHANGE = 1
    # fired when the same output is detected given n times
    REPEATED_THRESHOLD = 2

class Event:
    def __init__(self, category: TranscriptEvent, callback):
        self.category = category
        self.callback = callback

class TranscriptionService:
    def __init__(
            self,
            language = "pl",
            transcription_task = "transcribe",
            vad_parameters = None,
            model_size = "large-v2",
            device_index = 0,
            debug = False,
            print_in_loop = False,
            print_transcript = False,
            clear_before_transcript_print = False,
            excluded_phrases: Optional[List[str]] = []
        ):

        info(f"Cuda available: {torch.cuda.is_available()}")

        """ Transcription options """
        self.transcription_task = transcription_task
        self.language = language
        self.vad_parameters = vad_parameters
        self.model_size = model_size
        self.rate = 16000
        self.same_output_occurrence = 0
        self.same_output_threshold = 2
        self.speach_silence_threshold = 1
        self.min_sample_duration = 0.4 # 1.0
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
        self.device_index = device_index
        self.speach_to_text = WhisperModel(
            self.model_size,
            device_index = self.device_index,
            device=self.device,
            compute_type="float16" if self.device=="cuda" else "int8",
            local_files_only=False,
        )

        """ Phrases that will cause the segment to be dropped (not used in the final transcript).
        They come from the model being trained on silent audio chunks with subtitles in movies, YouTube videos, tv shows, etc."""
        self.excluded_phrases = excluded_phrases

        """ Transcription state """
        self._reset_on_next_tick = False
        self.audio_frames_buffer = None

        """ This is the text from the last received segment that is considered unfinished."""
        self.last_unfinished = ""

        self.prev_out = ""
        self.transcript = []
        self.t_start = None

        """ The value that increases continuously as the application processes audio data.
        It represents the cumulative amount of audio (in seconds) that has been processed from
        the start of the audio stream. It's like a marker moving forward as time progresses,
        indicating how far along the audio stream the application has processed.
        This value does not decrease; it only grows until the application shuts down or is reset. """
        self.timestamp_offset = 0.0

        """ Grows in specific increments, particularly when parts of the audio buffer are discarded
        to manage memory and focus on more recent audio data. This offset adjusts to account for the
        removal of older frames from the buffer, ensuring that the system knows the starting point
        of the current buffer relative to the original audio stream. It helps in recalibrating
        the system's understanding of "where" in the original audio stream the current buffer begins,
        especially after some data has been removed. """
        # TODO: check if this could cause stacking up memory
        self.frames_offset = 0.0

        """ Application settings """
        self.exit_flag = threading.Event()
        self.is_paused: bool = False
        self.debug: bool = debug
        self.print_in_loop: bool = print_in_loop
        self.print_transcript: bool = print_transcript
        self.clear_before_transcript_print: bool = clear_before_transcript_print
        self.print_first_samples_from_chunk: bool = False

        self.events: List[Event] = []

    def get_status_message(self):
        return f"Is paused: {self.is_paused}. Is set to exti: {self.exit_flag.is_set()}. Model: {self.model_size}. Device: {self.device}. Device index: {self.device_index}."

    def add_event_handler(self, category: TranscriptEvent, callback):
        if not isinstance(category, TranscriptEvent):
            raise ValueError("Invalid event type")

        self.events.append(Event(category, callback))

    def get_events_by_category(self, category: TranscriptEvent):
        return [event.callback for event in self.events if event.category == category]

    def on_silence(self):
        events = self.get_events_by_category(TranscriptEvent.SILENCE_AFTER)
        if len(events):
            text = self.get_full_transcript()
            reset_state = False
            for event in events:
                should_reset = event(text)
                if should_reset: reset_state = True
            if reset_state: self._reset_state()

    #  check if there is a silence in the audio input
    def check_silence(self):
        if self.t_start is None:
            if self.debug and self.print_in_loop: info("[LOOP] Started timer for silence detection")
            self.t_start = time.time()

        # has_speech = _info.duration_after_vad > 0
        if len(self.transcript) and self.transcript[-1] != '': # or not has_speech:
            if time.time() - self.t_start > self.speach_silence_threshold:
                if self.debug and self.print_in_loop:
                    info('[LOOP] Detected silence in audio input for {self.speach_silence_threshold} seconds. Adding a blank line to the transcript.')
                self.transcript.append('')
                self.on_silence()
                time.sleep(0.01)
                return True

        return False

    def clear(self):
        if os.name == "nt":
            os.system("cls")
            return

        os.system("clear")

        return cleared

    def set_language(self, language):
        if language == self.language: return

        self.language = language
        info(f"Language set to: {language}")

    def includes_excluded_phrase(self, text):
        clean_new_text = regex.sub(r'[^a-zA-Z]', '', text, flags=regex.UNICODE).lower()
        text_stripped = text.strip().lower()
        for phrase in self.excluded_phrases:
            clean_phrase = regex.sub(r'[^a-zA-Z]', '', phrase, flags=regex.UNICODE).lower()
            # if self.debug: info(f"Checking for excluded phrase: {clean_new_text} in {clean_phrase}")
            # print('------------------------------')
            # print(phrase.strip() , text.strip() , clean_phrase , clean_new_text)

            phrase_stripped = phrase.strip().lower()
            if phrase_stripped in text_stripped or text_stripped in phrase_stripped or clean_phrase in clean_new_text or clean_new_text in clean_phrase:
                return True

        return False

    def cleanup(self):
        self.exit_flag.set()

    def process_new_segments(self, new_segments, sample_duration):
        offset = None
        reached_repeat_threshold = False
        filtered_all_segments = False

        # filtered_segments = []
        # for segment in new_segments:
        #     # if self.debug: info(f"New segment: {segment.text}")
        #     if not self.includes_excluded_phrase(segment.text):
        #         filtered_segments.append(segment)
        #         if self.debug: info(f"Including phrase: {segment.text}")
        #     else:
        #         if self.debug: info(f"Excluded phrase detected: {segment.text}")

        if len(new_segments) == 0:
            filtered_all_segments = True
            return reached_repeat_threshold, filtered_all_segments

        # process complete segments except for the last one
        for i, s in enumerate(new_segments[:-1]):
            text_ = s.text
            self.transcript.append(text_)
            offset = min(sample_duration, s.end)

        self.last_unfinished = new_segments[-1].text

        # if same incomplete segment is seen multiple times then update the offset
        # and append the segment to the list
        if self.last_unfinished.strip() == self.prev_out.strip() and self.last_unfinished != '':
            self.same_output_occurrence += 1
        else:
            self.same_output_occurrence = 0

        if self.same_output_occurrence == self.same_output_threshold:
            if not len(self.transcript) or self.transcript[-1].strip().lower() != self.last_unfinished.strip().lower():
                self.transcript.append(self.last_unfinished)

            self.last_unfinished = ''
            offset = sample_duration
            self.same_output_occurrence = 0
            reached_repeat_threshold = True

        else:
            self.prev_out = self.last_unfinished

        # update offset
        if offset is not None:
            self.timestamp_offset += offset

        return reached_repeat_threshold, filtered_all_segments

    def get_confirmed_transcript(self):
        return ''.join(self.transcript)

    def get_full_transcript(self):
        # if self.debug and len(self.transcript) > 0: info(f"Transcript: {self.transcript}")
        # if self.debug and self.last_unfinished != '': info(f"Last unfinished: {self.last_unfinished}")
        return ''.join(self.transcript) + self.last_unfinished

    def transcribe(self, audio_data):
        """ This method will return either new segments or None. Mind that the speach_to_text method can also return None. if the VAD filter woul remove silence from the whole audio chunk. This means that the return value of this method needs to be always checked."""

        try:
            return self.speach_to_text.transcribe(
                audio_data,
                language = self.language,
                task = self.transcription_task,
                vad_filter = True,
                vad_parameters = self.vad_parameters,
                initial_prompt = self.last_unfinished
                )

        except Exception as e:
            error(f"Failed to transcribe audio chunk: {e}", exc_info=True)
            return None

    def transcription_loop(self):
        try:
            while not self.exit_flag.is_set():
                if self._reset_on_next_tick:
                    self._reset_state()
                    self._reset_on_next_tick = False

                if self.is_paused:
                    time.sleep(0.1)
                    continue

                current = self.get_confirmed_transcript()
                if self.debug and self.print_transcript and current != '':
                    if self.clear_before_transcript_print: self.clear()
                    info(f'[TRANSCRIPT]: {current}')

                # no point moving forward if we have no frames to process
                if self.audio_frames_buffer is None:
                    # if self.debug and self.print_in_loop: info("[LOOP] No frames to process")
                    time.sleep(0.01)
                    continue

                num_of_samples_to_skip = int((self.timestamp_offset - self.frames_offset) * self.rate)
                # Check if the current audio chunk exceeds duration of 25 seconds.
                if self.audio_frames_buffer[num_of_samples_to_skip:].shape[0] > 25 * self.rate:
                    # if self.debug and self.print_in_loop: info("[LOOP] Clipping audio chunk as it exceeds 30 seconds")

                    # Calculate the total duration of the audio in the buffer in seconds.
                    duration = self.audio_frames_buffer.shape[0] / self.rate

                    # Update the timestamp offset to be 5 seconds less than the total duration of the audio.
                    # This effectively "clips" the audio buffer to discard older audio data, keeping the most recent.
                    self.timestamp_offset = self.frames_offset + duration - 5

                # Select a single audio sample from the buffer to process.
                # if self.debug and self.print_in_loop: info("[LOOP] Selecting a single audio sample", True)
                samples_take = max(0, (self.timestamp_offset - self.frames_offset) * self.rate)
                # get sliced audio samples that exceed the sample_take
                input_bytes = self.audio_frames_buffer[int(samples_take):].copy()
                duration = input_bytes.shape[0] / self.rate

                if duration < self.min_sample_duration:
                    self.check_silence()
                    # ! this won't work
                    # self.timestamp_offset += duration
                    if self.debug and self.print_in_loop: info("[LOOP] sample below min duration")
                    time.sleep(0.01)
                    continue

                try:
                    if len(input_bytes) > 0:
                        input_sample = input_bytes.copy()

                        if self.debug and self.print_in_loop: info("[LOOP] started transcription")
                        result, _info = self.transcribe(input_sample)

                        if self.debug and self.print_in_loop: info(f'[LOOP] Sample duration: {_info.duration}, time filtered by VAD: {_info.duration - _info.duration_after_vad}, time left after VAD: {_info.duration_after_vad}')

                        if result is None:
                            if self.debug and self.print_in_loop: info("[LOOP] no result from transcription")
                            time.sleep(0.01)
                            continue

                        if _info.duration_after_vad == 0:
                            if self.debug and self.print_in_loop: info("[LOOP] VAD filtered out all audio, skipping segment")
                            # self.on_silence()
                            self.timestamp_offset += duration
                            time.sleep(0.01)
                            continue

                        if len(result) > 0:
                            if self.debug and self.print_in_loop: info("[LOOP] processing result")
                            self.t_start = None
                            old_transcript = self.get_full_transcript()
                            reached_repeat_threshold, filtered_all_segments = self.process_new_segments(result, duration)

                            if filtered_all_segments:
                                if self.debug and self.print_in_loop: info("[LOOP] all segments were filtered")
                                # self.on_silence()
                                # self.timestamp_offset += duration
                                time.sleep(0.01)
                                continue

                            new_transcript = self.get_full_transcript()
                            if old_transcript != new_transcript:
                                events = self.get_events_by_category(TranscriptEvent.TRANSCRIPTION_CHANGE)
                                if len(events):
                                    info("[EVENT]: transcription change detected")
                                    text = self.get_full_transcript()
                                    for event in events:
                                        event(text)

                            if reached_repeat_threshold:
                                if self.debug and self.print_in_loop: info("[EVENT]: reached repeat threshold")
                                events = self.get_events_by_category(TranscriptEvent.REPEATED_THRESHOLD)
                                if len(events):
                                    text = self.get_full_transcript()
                                    if self.debug and self.print_in_loop: info(f"Transcript on reached repeat threshold: {text}")
                                    reset_state = False
                                    for event in events:
                                        should_reset = event(text)
                                        if should_reset: reset_state = True
                                    if reset_state: self._reset_state()

                        else:
                            if self.check_silence():
                                    continue

                            if self.debug and self.print_in_loop: error("[LOOP] [ERROR] Transcription loop failed with error: no result from transcription")

                except Exception as e:
                    error(f" [LOOP]: Transcription failed with error: {e}", exc_info=True)
                    time.sleep(0.01)

        except Exception as e:
            error(f"[LOOP]: The transcription loop failed with error {e}", exc_info=True)


    def add_frames(self, new_frames):
        if self.audio_frames_buffer is not None:
            # if we have some frames and the total duration of the audio exceeds 45 seconds
            if self.audio_frames_buffer.shape[0] > 45 * self.rate:
                self.frames_offset += 30.0
                # remove all frames before the last 30 seconds
                self.audio_frames_buffer = self.audio_frames_buffer[int(30 * self.rate):]

        else:
            self.audio_frames_buffer = new_frames.copy()
            return

        self.audio_frames_buffer = numpy.concatenate((self.audio_frames_buffer, new_frames), axis=0)

    def _reset_state(self):
        self.transcript = []
        self.last_unfinished = ''
        self.prev_out = ''
        self.t_start = None
        self.timestamp_offset = 0.0
        self.audio_frames_buffer = None
        self.frames_offset = 0.0
        if self.debug: info("Reset to default state.")

    def reset_state(self):
        """ Reseting state from outside of the class should be done only at the specyfied point in the loop,
        othewise the cleaned data could be removed in the middle of the operation on that data. """
        self._reset_on_next_tick = True

    def pause(self):
        if self.debug: info("Pausing transcription.")
        self.is_paused = True

    def resume(self):
        if self.debug: info("Resuming transcription.")
        self.is_paused = False

