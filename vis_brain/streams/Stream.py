#-*- coding: utf-8

import numpy as np
import vis_brain.streams
import os


class Stream:
    '''
    Base class for all streams
    Stream is sequence of matrices each of which is saved to the hard disk at a certain timestamp during the
    simulation process.

    The is a base class for all classes that allow to read and record streams and create your own streams on Python

    Stream headers is a dictionary that tells you about impoertant stream properties. The headers may be revealed
    by the following command:
    stream.getHeaders()
    Stream properties are stored in the headers under following key:
    'height' - height of all matrices in the stream in pixels
    'width' - width of all matrices in the stream in pixels
    'nframes' - number of matrices in the stream (usually the same as total number of timestamps in the simulation)
    Takes no effect during writing of the stream but reads from the stream file during reading of the stream
    'current_frame' - number of frames currently opened or closed
    'height_um' - height of all matrices in the stream in um or any other native units
    'width_um' width of all matrices in the stream in um or any other native units
    'sample_rate' - sample rate for all stream matrices (usually the sample rate used during the simulation)
    'stream_mode' - 'read' if you want to open the stream for reading or 'write' if the stream shall be opened for
    writing. Of 'stream_mode' is set to 'read' the other stream properties have no effect since new headers will be
    loaded during read of the stream
    'opened' - False if the stream if closed or True if this is opened. The header is considered to be read-only,
    its set causes the stream data to be inaccucate
    If the stream is opened for reading these headers will be loaded from the stream file during the reading.
    However if you want to open stream for writing

    If you want to set stream headers, you may put a command like this:
    stream.getHeaders()["stream_mode"] = "read"

    Another possibility is to set some of stream headers during create of the stream:
    stream = vis_brain.streams.BinStream("my-file.bin", stream_mode = "read")

    To print all headers just use:
    print(stream)
    '''

    __headers = None
    _filename = None

    _headersDescription = {
        "height": "Height of all matrices in px",
        "width": "width of all matrices in px",
        "nframes": "total number of frames in the stream (no affect for writing streams)",
        "current_frame": "number of frames that has already been read or written",
        "height_um": "height of all matrices in the stream in um",
        "width_um": "width of all matrices in the stream in um",
        "sample_rate": "sample rate for stream data in Hz",
        "stream_mode": "Indicates whether stream is available for reading or for writing",
        "opened": ""
    }

    def __init__(self, filename, **kwargs):
        '''
        Creates new stream

        :param filename: name of a file associated for the stream. If the stream is going to be opened for reading this
        is the file where all stream data were stored. The file must exist. If the stream is going to be opened for
        writing this is the file where all stream matrices will be stored, this file shall not be exist.

        :param kwargs: there is a facility to set some of the stream properties during create of the stream. In order
        to do so, put parameters like
        '''
        from vis_brain.streams import get_default_stream_headers
        self._filename = filename
        self.__headers = {**get_default_stream_headers(), **kwargs}

    def getHeaders(self):
        '''
        :return: Stream headers as a dictionary
        '''
        return self.__headers

    def __str__(self):
        S = "Stream created for {0} with the following headers:".format(self._filename)
        S += "\nHeader\t\t\tValue\tComment"
        for key, value in self.__headers.items():
            S += "\n{0:20}\t{1}\t{2}".format(key, value, self._headersDescription[key])
        return S

    def open(self):
        '''
        Opens the stream for reading or writing depending on 'headers' value
        '''
        if self.__headers['opened']:
            return
        if self.__headers['stream_mode'] == "read":
            self._openForReading()
        if self.__headers['stream_mode'] == "write":
            if os.path.isfile(self._filename):
                raise vis_brain.streams.FileAlreadyExists()
            self._openForWriting()
        self.__headers['opened'] = True
        self.__headers['current_frame'] = 0

    def _openForReading(self):
        '''
        During creating custom streams this method shall be re-implemented in such a way as to
        open the stream file, read the file header and put its value to the stream header and
        save all necessary handles into private stream variables
        '''
        raise NotImplementedError("vis_brain.streams.Stream.Stream")

    def _openForWriting(self):
        '''
        During creating custom stream this method shall be re-implemented in such a way as it
        opens stream file for writing and fills the file header

        :return: nothing
        '''
        raise NotImplementedError("vis_brain.streams.Stream.Stream._openForWriting")

    def read(self):
        '''
        Reads a single matrix from the stream

        :return: the matrix itself as numpy object
        '''
        if not self.__headers['opened'] or self.__headers['stream_mode'] == 'write':
            raise vis_brain.streams.WrongStreamModeOrState()
        if self.__headers['current_frame'] >= self.__headers['nframes']:
            raise vis_brain.streams.EndOfStreamReached()
        try:
            matrix = self._read()
        except Exception as exc:
            self.close()
            raise exc
        self.__headers['current_frame'] += 1
        return matrix

    def move(self, n):
        '''
        Moves the pointer to the specified number of frames

        :return:
        '''
        if not self.__headers['opened']:
            raise vis_brain.streams.WrongStreamModeOrState()
        if self.__headers['current_frame'] + n < 0 or self.__headers['current_frame'] + n > self.__headers['nframes'] - 1:
            raise vis_brain.streams.EndOfStreamReached()
        try:
            self._move(n)
        except Exception as exc:
            self.close()
            raise exc
        self.__headers['current_frame'] += n

    def first(self):
        '''
        Moves the pointer to the initial frame

        :return: nothing
        '''
        if not self.__headers['opened']:
            raise vis_brain.streams.WrongStreamModeOrState()
        try:
            self._first()
        except Exception as exc:
            self.close()
            raise exc
        self.__headers['current_frame'] = 0

    def last(self):
        '''
        Moves the pointer to the last frame
        '''
        if not self.__headers['opened']:
            raise vis_brain.streams.WrongStreamModeOrState()
        try:
            self._last()
        except Exception as err:
            self.close()
            raise err
        self.__headers['current_frame'] = self.__headers['nframes'] - 1

    def _move(self, n):
        '''
        When you create custom stream from this base class you shall re-implement this method in such a way as it
        reads moves the pointer to the specified position

        :return:
        '''
        raise NotImplementedError("vis_brain.streams.Stream._move")

    def _first(self):
        '''
        When you create custom stream from this base class you shall re-implement this method in such a way as it
        moves to the beginning of the stream

        :return:
        '''
        raise NotImplementedError("vis_brain.streams.Stream._first")

    def _last(self):
        '''
        When you create custom stream from this base class you shall re-implement this method in such a way as it
        moves to the finish of the stream

        :return:
        '''
        raise NotImplementedError("vis_brain.streams.Stream._last")

    def _read(self):
        '''
        When you create custom stream from this base class you shall re-implement this method in such a way as it
        reads a single matrix from the stream and returns it

        :return: the matrix as numpy object
        '''
        raise NotImplementedError("vis_brain.streams.Stream.Stream._read")

    def write(self, matrix):
        '''
        Writes a single matrix into the stream

        :param matrix: matrix to write
        :return: nothing
        '''
        if not self.__headers['opened'] or self.__headers['stream_mode'] == "read":
            raise vis_brain.streams.WrongStreamModeOrState()
        desired_sizes = self.__headers["height"], self.__headers["width"]
        try:
            if matrix.shape != desired_sizes:
                raise ValueError("Matrix dimensions are not the same as stream dimensions")
            self._write(matrix)
        except Exception as exc:
            self.close()
            raise exc
        self.__headers['current_frame'] += 1

    def _write(self, matrix):
        '''
        When creating custom stream class from this base class this method shall be re-implemented in such a way as
        it writes the matrix directly to the stream
        '''
        raise NotImplementedError("vis_brain.streams.Stream.Stream._write")

    def close(self):
        '''
        Close the stream that is previously opened
        '''
        if not self.__headers['opened']:
            return
        self.__headers['opened'] = False
        if self.__headers['stream_mode'] == 'read':
            self._closeForReading()
        if self.__headers['stream_mode'] == 'write':
            self._closeForWriting()

    def _closeForReading(self):
        '''
        When you create custom stream from this base class you shall re-implement this method in such a way as it
        reads the stream footer and closes all file handles associated with this stream
        '''
        raise NotImplementedError("vis_brain.streams.Stream.Stream._closeForReading")

    def _closeForWriting(self):
        '''
        When you create custom stream from this base class you shall re-implement this method in such a way as it
        writes the stream footer and closes all file handles associated with this stream
        '''
        print("WRITING STREAM CLOSED")