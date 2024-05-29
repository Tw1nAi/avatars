import logging
import threading
import time

from custom_logger import error, info, warn
from transcription_server import ServerEvent, TranscriptionServer
from transcription_service import TranscriptEvent, TranscriptionService

logger = logging.getLogger()
logger.setLevel(logging.WARNING)
# logger.setLevel(logging.INFO)
# logger.setLevel(logging.DEBUG)

# ! use multiprocessing instead of threading to avoid Global Interpreter Lock (GIL)

if __name__ == "__main__":
    server = TranscriptionServer()
    transcription = TranscriptionService(
        language="pl",
        multilingual=True,
        debug=True,
        print_in_loop=True
    )

    def on_start_capture():
        transcription.reset_state()
        transcription.resume()
        server.receive_audio = True

    def on_stop_capture():
        server.receive_audio = False

    server.add_event_handler(ServerEvent.START_CAPTURE, on_start_capture)
    server.add_event_handler(ServerEvent.STOP_CAPTURE, on_stop_capture)
    server.add_event_handler(ServerEvent.NEW_FRAMES, transcription.add_frames)

    def on_final_transcript(transcript):
        info(f"Final transcript: {transcript}")
        server.send(f"[FINAL]{transcript}")
        transcription.pause()
        return True

    transcription.add_event_handler(TranscriptEvent.REPEATED_THRESHOLD, on_final_transcript)

    try:
        transcription_thread = threading.Thread(target=transcription.transcription_loop, daemon=True)
    except Exception as e:
        error(f"Could not initiate transcription thread: {e}")

    try:
        server_thread = threading.Thread(target=server.server.serve_forever, daemon=True)
    except Exception as e:
        error(f"Could not initiate websocket server thread: {e}")

    transcription_thread.start()
    info(f"Started transcription. {transcription.get_status_message()}")

    server_thread.start()
    info(f"Started websocket server on {server.host}:{server.port}")

    transcription.pause()
    transcription.reset_state()

    def end():
        server.server.shutdown()
        server.server.server_close()
        transcription.cleanup()
        info("Quitting application...")
        quit()

    try:
        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        end()

    except Exception as e:
        print(f"[EXCEPTION] The main loop exited with error: {e}")
        end()
