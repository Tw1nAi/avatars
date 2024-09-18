import argparse
import logging
import threading
import time

from custom_logger import error, info, warn
from transcription_server import ServerEvent, TranscriptionServer
from transcription_service import TranscriptEvent, TranscriptionService

logger = logging.getLogger()
# logger.setLevel(logging.WARNING)
logger.setLevel(logging.INFO)
# logger.setLevel(logging.DEBUG)

# ! use multiprocessing instead of threading to avoid Global Interpreter Lock (GIL)

excluded_phrases = [
    "PARROT TV",
    "Dziękuje za obejrzenie",
    "Thank you for your support on Patronite.",
    "Zapraszam na kolejny odcinek!",
    "Napisy stworzone przez społeczność Amara.org"
    "Amara.org",
    "Dzięki za uwagę",
    "Dzięki za obejrzenie!",
    "Wszystkie prawa zastrzeżone",
    "Transkrypcja Magdalena Świerczek-Gryboś",
    "Transkrypcja Magdalena Świerczek-Gryboś © PTA – TVP",\
    "PTA – TVP",
    "Dziękuję za uwagę",
    "Dziękuję za uwagę i do zobaczenia w kolejnym odcinku.",
    "Polskie Towarzystwo Astronomiczne",
    "Produkcja Polskie Towarzystwo Astronomiczne Telewizja Polska",
    "zapomnijcie zasubskrybować",
    "zafollowować",
    "Nie zapomnijcie zasubskrybować oraz zafollowować mnie na Facebooku!",
    "Zdjęcia i montaż",
    "Pracownia Prawa i Sprawiedliwość",
    "Teraz wracamy do naszego odcinka"
    "Teraz wracamy do naszego odcinka, w którym pokażę Wam",
    "najnowszych filmików na świecie",
    "Dziękuje za oglądanie i do zobaczenia w kolejnych odcinkach!"
]

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="A script to handle specific CLI arguments.")

    # Add arguments
    parser.add_argument('-l', '--language', type=str, default="pl", help='Set the initial transcription language (default: "pl")')
    parser.add_argument('-d', '--debug', action='store_false', help='Enable debug mode')
    parser.add_argument('-p', '--print_in_loop', action='store_false', help='Enable printing in a loop')

    # Parse the arguments
    args = parser.parse_args()

    print(f"Initial language: {args.language}")
    print(f"Debug: {args.debug}")
    print(f"Print in Loop: {args.print_in_loop}")

    server = TranscriptionServer()
    transcription = TranscriptionService(
        device_index=1,
        language=args.language,
        # multilingual=args.multilingual,
        debug=args.debug,
        # print_in_loop=args.print_in_loop,
        excluded_phrases=excluded_phrases
    )

    def on_start_capture(data):
        if "language" in data:
            transcription.set_language(data["language"])
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

    def on_silence_detected(transcript):
        info(f'Silence detected. Pausing transcription. Current transcript: {transcript}')
        server.send(f"[FINAL]{transcript}")
        server.receive_audio = False
        transcription.pause()
        return True

    transcription.add_event_handler(TranscriptEvent.REPEATED_THRESHOLD, on_final_transcript)
    transcription.add_event_handler(TranscriptEvent.SILENCE_AFTER, on_silence_detected)

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
