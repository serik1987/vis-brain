#-*- coding: utf-8

import wx

class SetLimitsWindow(wx.Dialog):

    __mainPanel = None
    __generalLayout = None
    __mainLayout = None
    __upperLayout = None
    __lowerLayout = None
    __btnOk = None
    __btnCancel = None
    __lowerLimitPrompt = None
    __lowerLimitBox = None
    __upperLimitPrompt = None
    __upperLimitBox = None

    __limits = None

    def __init__(self, parent, defaultLims):
        if defaultLims is None:
            defaultLims = (-1.0, 1.0)

        super().__init__(parent=parent, title="Set limits", size=(300, 500))
        self.__mainPanel = wx.Panel(self)
        self.__generalLayout = wx.BoxSizer(wx.VERTICAL)
        self.__mainLayout = wx.BoxSizer(wx.VERTICAL)

        self.__upperLayout = wx.FlexGridSizer(2, (5, 5))
        self.__upperLayout.AddGrowableCol(1, 1)

        self.__lowerLimitPrompt = wx.StaticText(self.__mainPanel, label="Min value")
        self.__upperLayout.Add(self.__lowerLimitPrompt, 0, wx.ALIGN_CENTRE_VERTICAL)

        self.__lowerLimitBox = wx.TextCtrl(self.__mainPanel, value=str(defaultLims[0]))
        self.__upperLayout.Add(self.__lowerLimitBox, 1, wx.EXPAND)

        self.__upperLimitPrompt = wx.StaticText(self.__mainPanel, label="Max value")
        self.__upperLayout.Add(self.__upperLimitPrompt, 0, wx.ALIGN_CENTRE_VERTICAL)

        self.__upperLimitBox = wx.TextCtrl(self.__mainPanel, value=str(defaultLims[1]))
        self.__upperLayout.Add(self.__upperLimitBox, 1, wx.EXPAND)

        self.__mainLayout.Add(self.__upperLayout, 1, wx.BOTTOM|wx.EXPAND, 10)

        self.__lowerLayout = wx.BoxSizer(wx.HORIZONTAL)

        self.__btnOk = wx.Button(self.__mainPanel, label="OK")
        self.Bind(wx.EVT_BUTTON, lambda event: self.finalize(), self.__btnOk)
        self.__lowerLayout.Add(self.__btnOk, 0, wx.RIGHT|wx.ALIGN_CENTRE_HORIZONTAL, 5)

        self.__btnCancel = wx.Button(self.__mainPanel, label="Cancel")
        self.Bind(wx.EVT_BUTTON, lambda event: self.Close(), self.__btnCancel)
        self.__lowerLayout.Add(self.__btnCancel, 0, wx.ALIGN_CENTRE_HORIZONTAL)

        self.__mainLayout.Add(self.__lowerLayout, 0, wx.ALIGN_CENTRE_HORIZONTAL, 0)

        self.__generalLayout.Add(self.__mainLayout, 1, wx.ALL|wx.EXPAND, 10)
        self.__mainPanel.SetSizer(self.__generalLayout)
        self.Centre()

        self.__generalLayout.Fit(self)
        height = self.GetSize().GetHeight()
        self.SetSize((300, height))

        self.__limits = defaultLims

    def finalize(self):
        try:
            minValue = float(self.__lowerLimitBox.GetValue())
            maxValue = float(self.__upperLimitBox.GetValue())
        except ValueError:
            wx.MessageBox("Either lower or upper limit is entered incorrectly", "Set limits",
                          wx.OK|wx.CENTRE|wx.ICON_ERROR)
            return
        if minValue < maxValue:
            self.__limits = minValue, maxValue
            self.EndModal(wx.ID_OK)
        else:
            wx.MessageBox("Lower limit can't be equal to or higher than the higher limit", "Set limits",
                          wx.OK|wx.CENTRE|wx.ICON_ERROR)

    def getLimits(self):
        return self.__limits