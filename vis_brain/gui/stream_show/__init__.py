#!/usr/bin/env python3

import sys
import wx
from vis_brain.gui.stream_show.MainWindow import MainWindow

if sys.version_info[0] < 3:
    raise Exception("This application is for python2, not python3")

if __name__ == "__main__":
    app = wx.App()
    if len(sys.argv) == 1:
        win = MainWindow()
    else:
        win = MainWindow(sys.argv[1])
    app.MainLoop()