#-*- coding: utf-8

import struct
import numpy as np
from vis_brain.readers import Loader

class BinReader(Loader):
    '''
    Implements reading and writing operations for .bin native vis-brain files
    '''
    __CHUNK_SIZE = 256
    __CHUNK = "#!vis-brain.data.reader"


    def read(self):
        '''
        Overrides a virtual mathod in the based class. For service use only
        '''
        chunk = self._file.read(self.__CHUNK_SIZE).decode("utf-8").strip('\x00')
        if chunk != self.__CHUNK:
            raise IOError("The file doesn't correspond to vis-brain .bin file or doesn't contain vis-brain static data")
        dimensions = self._file.read(24)
        width, height, widthUm, heightUm = struct.unpack("iidd", dimensions)
        self.setWidthUm(widthUm)
        self.setHeightUm(heightUm)
        size = width * height
        raw_data = self._file.read(8 * size)
        tupled_data = struct.unpack("d"*size, raw_data)
        linearized_data = np.array(tupled_data)
        data = np.reshape(linearized_data, (height, width))
        return data
