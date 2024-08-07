import json
import threading
import time
from enum import Enum

import numpy
from custom_logger import error, info, warn
from websockets.sync.server import serve


class ServerEvent(Enum):
    NEW_FRAMES = 0
    START_CAPTURE = 1
    STOP_CAPTURE = 2

class Event:
    def __init__(self, category: ServerEvent, callback):
        self.category = category
        self.callback = callback

class TranscriptionServer:
    def __init__(
            self,
            host: str =
                # "localhost",
                "0.0.0.0",
            port: int = 9090,
        ):

        """ Websocket server that receives audio stream """
        self.host = host
        self.port = port
        self.server = serve(
            self.on_client_connected,
            self.host,
            self.port
        )
        self.audio_stream_client = None
        self.consumer_client = None
        self.receive_audio = True

        """ Audio stream """
        self.events = []

        """ Application settings """
        self.exit_flag = threading.Event()

    def add_event_handler(self, category: ServerEvent, callback):
        if not isinstance(category, ServerEvent):
            raise ValueError("Invalid event type")

        self.events.append(Event(category, callback))

    def get_events_by_category(self, category: ServerEvent):
        return [event.callback for event in self.events if event.category == category]

    def receive_audio_stream(self, message):
        frame = numpy.frombuffer(message, dtype=numpy.float32)
        frame = numpy.clip(frame, -1.0, 1.0)

        events = self.get_events_by_category(ServerEvent.NEW_FRAMES)
        if len(events):
            for event in events:
                event(frame)

    def listen_audio_stream_client(self, websocket):
        while not self.exit_flag.is_set():
            try:
                if not self.receive_audio:
                    # without the timeout the thread would block execution all across the application
                    time.sleep(0.2)
                    continue

                if self.audio_stream_client:
                    message = websocket.recv()
                    if isinstance(message, str): continue
                    self.receive_audio_stream(message)

            except Exception as e:
                websocket.close()
                error(f"Error listen_audio_stream_client: {e}")
                del websocket
                break

    def listen_consumer_client(self, websocket):
        while not self.exit_flag.is_set():
            try:
                message = websocket.recv()

                if message is None or self.consumer_client is None:
                    continue

                if message == "stop_capture":
                    events = self.get_events_by_category(ServerEvent.STOP_CAPTURE)
                    if len(events):
                        for event in events:
                            event()
                    info("Received stop capture message.")
                    continue

                data = json.loads(message)
                if "message" in data:
                    if data["message"] == "start_capture":
                        events = self.get_events_by_category(ServerEvent.START_CAPTURE)
                        if len(events):
                            for event in events:
                                event(data)
                        info("Received start capture message.")
                        continue


            except Exception as e:
                error(f"Error in listen_consumer_client: {e}")
                self.consumer_client = None
                websocket.close()
                del websocket
                break

    def on_client_connected(self, websocket):
        message = websocket.recv()
        if isinstance(message, str): info(f"On connected message: {message}")

        if message == "open_audio_stream":
            # allow only a single stream client
            if self.audio_stream_client:
                self.audio_stream_client.close()
                self.audio_stream_client = None
                info("Disconnected current audio stream client")
            self.audio_stream_client = websocket
            info("Audio stream client connected")
            self.listen_audio_stream_client(websocket)
            return

        if message == "text_consumer":
            # allow only a single consumer client
            if self.consumer_client:
                self.consumer_client.close()
                self.consumer_client = None
                info("Disconnected current consumer client")
            self.consumer_client = websocket
            info("Text consumer client connected")
            time.sleep(0.2) # give current consumer client time to close
            self.listen_consumer_client(websocket)
            return

    def send(self, message):
        if self.consumer_client:
            self.consumer_client.send(message)
        else:
            warn("No consumer client connected")
