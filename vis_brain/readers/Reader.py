#-*- coding: utf-8


class Reader:
    '''
    A base class that reads the data saved by the vis-brain and saves the data into a format appropriate by their
    reading through the vis-brain
    '''
    __filename = None
    __widthUm = float('nan')
    __heightUm = float('nan')

    def __init__(self, filename):
        '''
        Common constructor for all readers
        :param filename: default filename
        '''
        self.__filename = filename

    def getFilename(self):
        '''
        Returns the default filename
        '''
        return self.__filename

    def setFilename(self, filename):
        '''
        Sets the default filename
        '''
        self.__filename = filename

    def getWidthUm(self):
        '''
        returns the matrix width in um or any other native units
        '''
        return self.__widthUm

    '''
    Returns the matrix height in um or any other native units
    '''
    def getHeightUm(self):
        return self.__heightUm

    def setWidthUm(self, value):
        '''
        Sets the matrix width in um or any other native units
        '''
        self.__widthUm = float(value)
        if self.__widthUm <= 0:
            raise ValueError("wrong value for widthUm property")

    def setHeightUm(self, value):
        '''
        Sets the matrix height in um or any other native units
        '''
        self.__heightUm = float(value)
        if self.__heightUm <= 0:
            raise ValueError("Wrong value for heightUm property")

    def __str__(self):
        S = "vis-brain reader\n"
        S += "Filename to read: {0}\n".format(self.getFilename())
        S += "Matrix width (um): {0}\n".format(self.getWidthUm())
        S += "Matrix height (um): {0}\n".format(self.getHeightUm())
        return S