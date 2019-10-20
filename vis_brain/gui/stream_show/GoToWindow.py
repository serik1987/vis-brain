#-*- coding: utf-8

import wx

class GoToWindow(wx.Dialog):

    __mainPanel = None
    __generalLayout = None
    __mainLayout = None
    __upperLayout = None
    __lowerLayout = None
    __frameNumberBox = None

    __stream = None
    __frameNumber = None

    def __init__(self, parent, stream):
        super().__init__(parent=parent, title="Go to frame", size=(200,500))
        self.__mainPanel = wx.Panel(self)
        self.__generalLayout = wx.BoxSizer(wx.VERTICAL)
        self.__mainLayout = wx.BoxSizer(wx.VERTICAL)

        self.__upperLayout = wx.BoxSizer(wx.HORIZONTAL)
        txt = wx.StaticText(self.__mainPanel, label="# frame")
        self.__upperLayout.Add(txt, 0, wx.ALIGN_CENTRE_VERTICAL|wx.RIGHT, 5)
        self.__frameNumberBox = wx.TextCtrl(self.__mainPanel, style=wx.TE_PROCESS_ENTER)
        self.Bind(wx.EVT_TEXT_ENTER, lambda event: self.validate(), self.__frameNumberBox)
        self.__upperLayout.Add(self.__frameNumberBox, 1, wx.EXPAND)
        self.__mainLayout.Add(self.__upperLayout, 1, wx.BOTTOM|wx.EXPAND, 10)

        self.__lowerLayout = wx.BoxSizer(wx.HORIZONTAL)
        okBtn = wx.Button(self.__mainPanel, label="OK")
        self.Bind(wx.EVT_BUTTON, lambda event: self.validate(), okBtn)
        self.__lowerLayout.Add(okBtn, 0, wx.RIGHT|wx.ALIGN_CENTRE_HORIZONTAL, 5)
        cancelBtn = wx.Button(self.__mainPanel, label="Cancel")
        self.Bind(wx.EVT_BUTTON, lambda event: self.Close(), cancelBtn)
        self.__lowerLayout.Add(cancelBtn, 0, wx.ALIGN_CENTRE_HORIZONTAL)
        self.__mainLayout.Add(self.__lowerLayout, 0, wx.ALIGN_CENTRE_HORIZONTAL)

        self.__generalLayout.Add(self.__mainLayout, 1, wx.ALL|wx.EXPAND, 10)
        self.__mainPanel.SetSizer(self.__generalLayout)
        self.Centre(wx.BOTH)
        self.__generalLayout.Fit(self)

        self.__stream = stream

    def validate(self):
        try:
            self.__frameNumber = int(self.__frameNumberBox.GetValue())
        except ValueError:
            wx.MessageBox("Please, enter a valid integer value", "Frame # error", wx.OK|wx.CENTRE|wx.ICON_ERROR)
            return
        if 0 <= self.__frameNumber < self.__stream.getHeaders()['nframes']:
            self.EndModal(wx.ID_OK)
        else:
            wx.MessageBox("Please, enter a number within the range from 0 to {0}"
                          .format(self.__stream.getHeaders()["nframes"]-1), "Frame # error", wx.OK|wx.CENTRE|wx.ICON_ERROR)

    def getFrameNumber(self):
        return self.__frameNumber