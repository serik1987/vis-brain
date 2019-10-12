#-*- coding: utf-8

from vis_brain.readers import Reader


class Loader(Reader):
    '''
    Base class for all readers that support file loading
    '''

    _file = None

    def load(self, filename = ""):
        '''
        Loads the file

        filename - name of the loading file or "" is one shall use the default filename
        '''
        if filename == "":
            current_filename = self.getFilename()
        else:
            current_filename = filename
        self._file = open(current_filename, 'rb')
        matrix = self.read()
        self._file.close()
        return matrix

    def read(self):
        '''
        Reads the data from the file

        This is a virtual method that implements a certain reading algorithm based on the file structure. In this class
        the method is based however, in derived class it shall be re-implemented. In this case self._file variable
        will represent an opened file that this method shall read

        The method shall return a matrix (numpy object)
        '''
        raise NotImplementedError("vis_brain.readers.Loader.read")