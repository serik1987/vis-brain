#-*- coding: utf-8

import wx

class StreamPropertiesWindow(wx.Dialog):

    def __init__(self, parent, stream):
        super().__init__(parent=parent, title="Stream properties")
        panel = wx.Panel(self)
        generalSizer = wx.BoxSizer(wx.VERTICAL)
        mainPanel = wx.BoxSizer(wx.VERTICAL)

        self.__printStreamInfo(panel, mainPanel, stream)

        okButton = wx.Button(panel, label="OK")
        self.Bind(wx.EVT_BUTTON, lambda event: self.EndModal(wx.OK), okButton)
        mainPanel.Add(okButton, 0, wx.ALIGN_CENTRE_HORIZONTAL)

        generalSizer.Add(mainPanel, 1, wx.ALL|wx.EXPAND, 10)
        panel.SetSizer(generalSizer)
        generalSizer.Fit(self)
        self.Centre(wx.BOTH)

    def __printStreamInfo(self, parentPanel, sizer, stream):
        streamProperties = str(stream).split("\n")[1:]
        infoSizer = wx.FlexGridSizer(3, (5, 5))
        infoSizer.AddGrowableCol(2, 1)

        for streamProperty in streamProperties:
            for propertyField in streamProperty.split("\t"):
                propertyField = propertyField.strip()
                if propertyField != "":
                    window = wx.StaticText(parentPanel, label=propertyField)
                    infoSizer.Add(window, 0, wx.ALIGN_LEFT)

        sizer.Add(infoSizer, 1, wx.BOTTOM|wx.EXPAND, 10)