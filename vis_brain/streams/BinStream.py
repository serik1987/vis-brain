#-*- coding: utf-8

import numpy as np
import struct
from vis_brain.streams.Stream import Stream

_STREAM_CHUNK = "#!vis-brain.data.stream"
_STREAM_CHUNK_SIZE = 256

class BinStream(Stream):
    '''
    Represents so called .bin stream. The .bin stream deals with native vis-brain .bin's streams. All routines
    gave already been declared in the Stream class. This class declares their implementations
    '''

    __handle = None

    def _openForReading(self):
        global _STREAM_CHUNK, _STREAM_CHUNK_SIZE
        self.__handle = open(self._filename, "rb")
        try:
            raw_chunk = self.__handle.read(_STREAM_CHUNK_SIZE)
            chunk = raw_chunk.decode("utf-8").rstrip("\0")
            if chunk != _STREAM_CHUNK:
                raise IncorrectFileFormat()
            raw_header = self.__handle.read(12)
            header = struct.unpack("iii", raw_header)
            self.getHeaders()['height'] = header[0]
            self.getHeaders()['width'] = header[1]
            self.getHeaders()['nframes'] = header[2]
            raw_header = self.__handle.read(24)
            header = struct.unpack("ddd", raw_header)
            self.getHeaders()['height_um'] = header[0]
            self.getHeaders()['width_um'] = header[1]
            self.getHeaders()['sample_rate'] = header[2]
        except Exception as exc:
            self.__handle.close()
            raise exc

    def _read(self):
        height = self.getHeaders()['height']
        width = self.getHeaders()['width']
        size = height * width
        frame_size = size * 8
        raw_data = self.__handle.read(frame_size)
        data = struct.unpack("d"*size, raw_data)
        raw_matrix = np.array(data)
        matrix = np.reshape(raw_matrix, (height, width))
        return matrix

    def _closeForReading(self):
        self.__handle.close()


class IncorrectFileFormat(IOError):
    def __init__(self):
        super().__init__("The file you point to is not the vis-brain stream file")