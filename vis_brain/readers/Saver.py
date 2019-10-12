#-*- coding: utf-8

from vis_brain.readers import Reader

class Saver(Reader):
    '''
    A base class for all readers that allow the data writing
    '''

    _savingFile = None

    def save(self, matrix, filename = ""):
        '''
        Saves the matrix. An instance uses widthUm and heightUm properties to specify the width and the height in um

        :param matrix: matrix to be saved
        :param filename: filename to save or empty string if saving to default file is required
        :return: None
        '''
        if filename == "":
            current_filename = self.getFilename()
        else:
            current_filename = filename
        self._savingFile = open(current_filename, "wb")
        self.write(matrix)
        self._savingFile.close()

    def write(self, matrix):
        raise NotImplementedError("vis_brain.readers.Saver.write")