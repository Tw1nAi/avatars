import logging

__all__ = ['logger', 'info', 'warn', 'error']

# Your custom handler
class CountingLogHandler(logging.StreamHandler):
    def __init__(self):
        super().__init__()
        self.last_record = None
        self.repeat_count = 1

    def emit(self, record):
        formatted_message = self.format(record)
        if self.last_record == formatted_message:
            self.repeat_count += 1
            # Clear the previous line if it's the same message (only if the terminal supports it)
            if self.is_tty():
                self.stream.write("\033[F\033[K")  # ANSI escape sequence to move cursor up and clear the line
            self.stream.write(f"({self.repeat_count}) {formatted_message}\n")
        else:
            if self.repeat_count > 1:
                self.repeat_count = 1  # Reset repeat count
            self.stream.write(f"{formatted_message}\n")
            self.last_record = formatted_message  # Update last record
        self.flush()

    def is_tty(self):
        """ Check if the stream is attached to a terminal/tty. """
        return hasattr(self.stream, 'isatty') and self.stream.isatty()

# Configure the root logger
def setup_logger():
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)  # Set the level you need for the root logger
    # Adding a handler to the root logger
    handler = CountingLogHandler()
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(name)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    return logger

# Instantiating and configuring the logger immediately
logger = setup_logger()

def info(*args):
    logger.info(*args)

def warn(*args):
    logger.warn(*args)

def error(*args):
    logger.error(*args)
