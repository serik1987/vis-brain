#-*- coding: utf-8

from vis_brain.readers.Reader import Reader
from vis_brain.readers.Loader import Loader
from vis_brain.readers.Saver import Saver
from vis_brain.readers.BinReader import BinReader

def create_reader(format, filename):
    '''
    chooses an appropriate reader and creates it

    :param format: file format: 'bin' for native vis-brain .bin files
    :param filename: filename
    :return: the reader
    '''
    if format == "bin":
        from vis_brain.readers.BinReader import BinReader
        reader_class = BinReader
    else:
        raise NotImplementedError("Unknown or unsupported file format")
    return reader_class(filename)