import logging


def enable_log(func):
    def wrapper(*args, **kwargs):
        logger = logging.getLogger()
        logger.setLevel(logging.DEBUG)
        func(*args, **kwargs)
        logger.setLevel(logging.WARNING)
    return wrapper

