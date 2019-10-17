#-*- coding: utf-8

from vis_brain.streams.BinStream import BinStream


class FileAlreadyExists(IOError):
    def __init__(self):
        super().__init__("Can't create new stream with the same name as existing stream")


class WrongStreamModeOrState(IOError):
    def __init__(self):
        super().__init__("Wrong stream mode or state")


class EndOfStreamReached(IOError):
    def __init__(self):
        super().__init__("End of stream has been reached: further read from the stream is impossible")


def get_default_stream_headers():
    '''
    Returns a list of default stream headers
    :return:
    '''
    return {
        "height": 512,
        "width": 512,
        "nframes": 40,
        "current_frame": 0,
        "height_um": 12000.0,
        "width_um": 12000.0,
        "sample_rate": 1.0,
        "stream_mode": "write",
        "opened": False
    }