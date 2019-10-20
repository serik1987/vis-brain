#-*- coding: utf-8

import os
import wx
import numpy as np
import matplotlib
matplotlib.use("WXAgg")

from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas
from matplotlib.figure import Figure

class MainWindow(wx.Frame):
    MENU_ITEM_LOAD_STREAM = 0x00
    MENU_ITEM_CLOSE_STREAM = 0x01
    MENU_ITEM_DRAW_ALL_FRAMES = 0x02
    MENU_ITEM_EXPORT_ALL_FRAMES = 0x03
    MENU_ITEM_STREAM_PROPERTIES = 0xFE
    MENU_ITEM_EXIT = 0x04
    MENU_ITEM_GO_TO_FIRST = 0x05
    MENU_ITEM_GO_TO_LAST = 0x06
    MENU_ITEM_GO_TO_PREVIOUS = 0x07
    MENU_ITEM_GO_TO_NEXT = 0x08
    MENU_ITEM_GO_TO = 0x09
    MENU_ITEM_FRAME_SAVE = 0x0A
    MENU_ITEM_PLAY = 0x0B
    MENU_ITEM_STOP = 0x0C
    MENU_ITEM_COLORMAP = 0xFF
    MENU_ITEM_COLORMAP_JET = 0x0D
    MENU_ITEM_COLORMAP_HSV = 0x0E
    MENU_ITEM_COLORMAP_GRAY = 0x0F
    MENU_ITEM_COLORBAR = 0x10
    MENU_ITEM_SET_LIMITS = 0x11

    TOOL_FIRST = 0x20
    TOOL_PREVIOUS = 0x21
    TOOL_NEXT = 0x22
    TOOL_LAST = 0x23

    __figure = None
    __canvas = None
    __clim = -1.0, 1.0
    __timer = None

    __filename = None
    __stream = None
    __frame = None

    def __init__(self, filename = None):
        super().__init__(parent=None, title="Stream view", size=(700, 500))
        self.__createMenuBar()
        self.__createToolbar()
        self.__createAxes()
        self.CreateStatusBar()
        self.__timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, lambda event: self.autoNext(), self.__timer)
        self.Centre(wx.BOTH)
        self.SetMinSize((200, 100))
        self.Show()
        self.drawClosedState()
        if filename is not None:
            self.loadStream(filename)
        self.Bind(wx.EVT_CLOSE, lambda event: self.onClose())

    def onClose(self):
        self.closeStream()
        self.Destroy()

    def __createMenuBar(self):
        mainMenu = wx.MenuBar()

        fileMenu = wx.Menu()
        fileLoad = fileMenu.Append(self.MENU_ITEM_LOAD_STREAM, "L&oad stream...\tCTRL+O",
                                   "Loads stream from the hard disk", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.loadStreamCallback(), fileLoad)
        fileClose = fileMenu.Append(self.MENU_ITEM_CLOSE_STREAM, "&Close stream\tCTRL+W",
                                    "Coses the stream", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.closeStream(), fileClose)
        fileMenu.AppendSeparator()
        fileDraw = fileMenu.Append(self.MENU_ITEM_DRAW_ALL_FRAMES, "&Draw all frames...",
                                   "Saves each frame of the stream as an image in the certain folder", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.drawAllFramesCallback(), fileDraw)
        fileExport = fileMenu.Append(self.MENU_ITEM_EXPORT_ALL_FRAMES, "Export all frames...",
                                     "Saves all frames as data stored in a MATLAB file", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.exportAllFramesCallback(), fileExport)
        fileStreamProperties = fileMenu.Append(self.MENU_ITEM_STREAM_PROPERTIES, "Stream &properties\tALT+ENTER",
                                               "Prints all stream properties...", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.getStreamProperties(), fileStreamProperties)
        fileMenu.AppendSeparator()
        fileExit = fileMenu.Append(self.MENU_ITEM_EXIT, "&Exit\tCTRL+Q", "Quits from the program", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.Close(), fileExit)
        mainMenu.Append(fileMenu, "&File")

        frameMenu = wx.Menu()
        frameFirst = frameMenu.Append(self.MENU_ITEM_GO_TO_FIRST, "&First frame\tHOME",
                                      "Moves to the first frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.goToFirst(), frameFirst)
        framePrevious = frameMenu.Append(self.MENU_ITEM_GO_TO_PREVIOUS, "&Previous frame\tKP_4",
                                         "Moves to the previous frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.goToPrevious(), framePrevious)
        frameNext = frameMenu.Append(self.MENU_ITEM_GO_TO_NEXT, "&Next frame\tKP_6",
                                     "Moves to the next frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.goToNext(), frameNext)
        frameLast = frameMenu.Append(self.MENU_ITEM_GO_TO_LAST, "&Last frame\tEND",
                                     "Moves to the last frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.goToLast(), frameLast)
        frameGoto = frameMenu.Append(self.MENU_ITEM_GO_TO, "&Go to...\tCTRL+G",
                                     "Moves to the specified frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.goToCallback(), frameGoto)
        frameMenu.AppendSeparator()
        frameSave = frameMenu.Append(self.MENU_ITEM_FRAME_SAVE, "&Save frame...\tCTRL+S",
                                     "Saves a certain frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.saveFrameCallback(), frameSave)
        mainMenu.Append(frameMenu, "F&rame")

        viewMenu = wx.Menu()
        viewPlay = viewMenu.Append(self.MENU_ITEM_PLAY, "&Play...\tCTRL+P",
                                   "Plays the stream", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.play(), viewPlay)
        viewStop = viewMenu.Append(self.MENU_ITEM_STOP, "&Stop...\tCTRL+L",
                                   "Stops playing of the stream", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.stop(), viewStop)
        viewMenu.AppendSeparator()

        colormapMenu = wx.Menu()
        colormapJet = colormapMenu.Append(self.MENU_ITEM_COLORMAP_JET, "&Jet",
                                          "Specifies colored image representing non-periodic values", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, lambda event: self.jet(), colormapJet)
        colormapHsv = colormapMenu.Append(self.MENU_ITEM_COLORMAP_HSV, "&Hsv",
                                          "Speficies colored image representing periodic values", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, lambda event: self.hsv(), colormapHsv)
        colormapGray = colormapMenu.Append(self.MENU_ITEM_COLORMAP_GRAY, "&Gray",
                                           "Specifies black-and-white image for non-pediodic values", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, lambda event: self.gray(), colormapGray)
        viewMenu.Append(self.MENU_ITEM_COLORMAP, "Color&map",
                        colormapMenu, "Specifies color codes for the matrix values")
        colormapMenu.Check(self.MENU_ITEM_COLORMAP_JET, True)

        viewColorbar = viewMenu.Append(self.MENU_ITEM_COLORBAR, "Color&bar",
                                       "Toggles on/off colorbar at the right of the frame image", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.colorbarCallback, viewColorbar)
        viewSetLimits = viewMenu.Append(self.MENU_ITEM_SET_LIMITS, "Set &limits...",
                                        "Sets minimum and maximum value for the color axis", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_MENU, lambda event: self.setLimitsCallback(), viewSetLimits)
        mainMenu.Append(viewMenu, "&View")

        self.SetMenuBar(mainMenu)

    def __createToolbar(self):
        toolbar = self.CreateToolBar()
        current_dir = os.path.dirname(__file__)

        first_icon_filename = os.path.join(current_dir, "beginning-media-button-arrow.svg")
        first_icon = wx.Bitmap(first_icon_filename)
        first = toolbar.AddTool(self.TOOL_FIRST, "Go to first frame", first_icon,
                                "Moves to the first frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_TOOL, lambda event: self.goToFirst(), first)

        previous_icon_filename = os.path.join(current_dir, "left-arrow.svg")
        previous_icon = wx.Bitmap(previous_icon_filename)
        previous = toolbar.AddTool(self.TOOL_PREVIOUS, "Go to previous frame", previous_icon,
                                   "Moves to the previous frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_TOOL, lambda event: self.goToPrevious(), previous)

        next_icon_filename = os.path.join(current_dir, "right-arrow.svg")
        next_icon = wx.Bitmap(next_icon_filename)
        next = toolbar.AddTool(self.TOOL_NEXT, "Go to next frame", next_icon,
                               "Moves to the next frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_TOOL, lambda event: self.goToNext(), next)

        last_icon_filename = os.path.join(current_dir, "finishing-media-button-arrow.svg")
        last_icon = wx.Bitmap(last_icon_filename)
        last = toolbar.AddTool(self.TOOL_LAST, "Go to last frame", last_icon,
                               "Moves to the last frame", wx.ITEM_NORMAL)
        self.Bind(wx.EVT_TOOL, lambda event: self.goToLast(), last)

        toolbar.Realize()

    def __createAxes(self):
        panel = wx.Panel(self)

        self.__figure = Figure()
        self.__axes = self.__figure.add_subplot(111)
        self.__canvas = FigureCanvas(panel, -1, self.__figure)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.__canvas, 1, wx.ALL | wx.EXPAND, 10)
        panel.SetSizer(sizer)
        panel.Fit()

    def drawClosedState(self):
        self.SetTitle("Stream view")
        menu = self.GetMenuBar()
        menu.Enable(self.MENU_ITEM_CLOSE_STREAM, False)
        menu.Enable(self.MENU_ITEM_DRAW_ALL_FRAMES, False)
        menu.Enable(self.MENU_ITEM_EXPORT_ALL_FRAMES, False)
        menu.Enable(self.MENU_ITEM_STREAM_PROPERTIES, False)
        menu.EnableTop(1, False)
        menu.EnableTop(2, False)

        toolbar = self.GetToolBar()
        toolbar.EnableTool(self.TOOL_FIRST, False)
        toolbar.EnableTool(self.TOOL_LAST, False)
        toolbar.EnableTool(self.TOOL_PREVIOUS, False)
        toolbar.EnableTool(self.TOOL_NEXT, False)

        self.__figure.clf()
        self.__canvas.draw()

    def drawFrame(self):
        self.SetTitle("{0} - Stream view".format(os.path.basename(self.__filename)))
        menu = self.GetMenuBar()
        tools = self.GetToolBar()

        menu.Enable(self.MENU_ITEM_CLOSE_STREAM, True)
        menu.Enable(self.MENU_ITEM_DRAW_ALL_FRAMES, True)
        menu.Enable(self.MENU_ITEM_EXPORT_ALL_FRAMES, True)
        menu.Enable(self.MENU_ITEM_STREAM_PROPERTIES, True)
        menu.EnableTop(1, True)
        menu.EnableTop(2, True)
        tools.EnableTool(self.TOOL_FIRST, True)
        tools.EnableTool(self.TOOL_LAST, True)

        current_frame = self.__stream.getHeaders()['current_frame']
        if current_frame > 1:
            menu.Enable(self.MENU_ITEM_GO_TO_PREVIOUS, True)
            tools.EnableTool(self.TOOL_PREVIOUS, True)
        else:
            menu.Enable(self.MENU_ITEM_GO_TO_PREVIOUS, False)
            tools.EnableTool(self.TOOL_PREVIOUS, False)

        if current_frame < self.__stream.getHeaders()['nframes']:
            menu.Enable(self.MENU_ITEM_GO_TO_NEXT, True)
            tools.EnableTool(self.TOOL_NEXT, True)
        else:
            menu.Enable(self.MENU_ITEM_GO_TO_NEXT, False)
            tools.EnableTool(self.TOOL_NEXT, False)

        self.__figure.clf()
        ax = self.__figure.add_subplot(111)
        img = ax.imshow(self.__frame, cmap=self.getColormap(), vmin=self.__clim[0], vmax=self.__clim[1])
        ax.set_aspect("equal")
        time = 1000 * (current_frame - 1) / self.__stream.getHeaders()['sample_rate']
        ax.set_title("t = {0:.2f} ms".format(time))
        if self.isColorbar():
            self.__figure.colorbar(img)
        self.__canvas.draw()

    def getColormap(self):
        menu = self.GetMenuBar()
        if menu.IsChecked(self.MENU_ITEM_COLORMAP_JET):
            return "jet"
        if menu.IsChecked(self.MENU_ITEM_COLORMAP_HSV):
            return "hsv"
        if menu.IsChecked(self.MENU_ITEM_COLORMAP_GRAY):
            return "gray"

    def isColorbar(self):
        return self.GetMenuBar().IsChecked(self.MENU_ITEM_COLORBAR)

    def loadStreamCallback(self):
        dlg = wx.FileDialog(self, "Open the stream file", style=wx.FD_OPEN|wx.FD_FILE_MUST_EXIST)
        result = dlg.ShowModal()
        if result == wx.ID_CANCEL:
            return
        filename = dlg.GetPath()
        self.loadStream(filename)

    def loadStream(self, filename):
        self.closeStream()
        import vis_brain.streams
        import inspect
        from vis_brain.streams.Stream import Stream
        for module_name in dir(vis_brain.streams):
            instance = getattr(vis_brain.streams, module_name)
            if inspect.isclass(instance):
                if issubclass(instance, Stream):
                    try:
                        self.__loadSpecifiedStream(filename, instance)
                        return
                    except Exception as exc:
                        print("Stream class {0} gave {1}. Trying another class".format(instance.__name__, exc))
        wx.MessageDialog(self, "Unsupported stream format or error opening the stream", "Load stream",
                         wx.OK|wx.ICON_ERROR|wx.CENTRE).ShowModal()

    def __loadSpecifiedStream(self, filename, streamclass):
        stream = streamclass(filename, stream_mode="read")
        stream.open()
        self.__filename = filename
        self.__stream = stream
        self.__frame = stream.read()
        self.drawFrame()

    def closeStream(self):
        if self.__stream is not None and self.__stream.getHeaders()['opened']:
            self.__stream.close()
            self.drawClosedState()

    def drawAllFramesCallback(self):
        dlg = wx.DirDialog(self, "Folder where a set of PNG files shall be located")
        if dlg.ShowModal() == wx.ID_OK:
            dirname = dlg.GetPath()
            if not os.path.isdir(dirname):
                os.mkdir(dirname)
            self.drawAllFrames(dirname)

    def drawAllFrames(self, dirname):
        self.goToFirst()
        while self.__stream.getHeaders()['current_frame'] < self.__stream.getHeaders()['nframes']:
            time = (self.__stream.getHeaders()['current_frame'] - 1) *1000.0  / self.__stream.getHeaders()['sample_rate']
            short_file = "t={0}ms.png".format(time)
            full_file = os.path.join(dirname, short_file)
            self.__figure.savefig(full_file)
            self.GetStatusBar().SetStatusText("Saving {0} ms...".format(time))
            self.goToNext()
        self.GetStatusBar().SetStatusText("")

    def exportAllFramesCallback(self):
        dlg = wx.FileDialog(self, "Export all frames to .mat file", wildcard="MAT files (.mat)|*.mat",
                            style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT)
        if dlg.ShowModal() == wx.ID_OK:
            filename = dlg.GetPath()
            if filename[-4:] != ".mat":
                filename += ".mat"
            self.exportAllFrames(filename)

    def exportAllFrames(self, filename):
        import scipy.io
        output_data = {
            "stream_name": self.__filename,
            "times": [],
            "time_units": "ms",
            "frames": [],
            "nframes": self.__stream.getHeaders()['nframes'],
            "sample_rate": self.__stream.getHeaders()['sample_rate'],
            "height": self.__stream.getHeaders()['height'],
            "width": self.__stream.getHeaders()['width'],
            "height_um": self.__stream.getHeaders()['height_um'],
            "width_um": self.__stream.getHeaders()['width_um']
        }
        print(self.__stream)
        self.__stream.first()
        while self.__stream.getHeaders()['current_frame'] < self.__stream.getHeaders()['nframes']:
            time = self.__stream.getHeaders()['current_frame'] * 1000 / self.__stream.getHeaders()['sample_rate']
            self.GetStatusBar().SetStatusText("Exporting t = {0} ms".format(time))
            output_data["times"].append(time)
            frame = self.__stream.read()
            output_data["frames"].append(frame)
        output_data['times'] = np.array(output_data['times'])
        scipy.io.savemat(filename, output_data)
        self.GetStatusBar().SetStatusText("")
        self.goToFirst()

    def getStreamProperties(self):
        from .StreamPropertiesWindow import StreamPropertiesWindow
        dlg = StreamPropertiesWindow(self, self.__stream)
        dlg.ShowModal()

    def goToFirst(self):
        try:
            self.__stream.first()
            self.__frame = self.__stream.read()
            self.drawFrame()
        except Exception as err:
            wx.MessageBox(str(err), "Moving to the beginning of the stream", wx.OK | wx.ICON_ERROR | wx.CENTRE)

    def goToPrevious(self):
        try:
            self.__stream.move(-2)
            self.__frame = self.__stream.read()
            self.drawFrame()
        except Exception as err:
            wx.MessageBox(str(err), "Moving to the previous frame", wx.OK | wx.ICON_ERROR | wx.CENTRE)

    def goToNext(self):
        try:
            self.__frame = self.__stream.read()
            self.drawFrame()
        except Exception as err:
            wx.MessageBox(str(err), "Moving to the next frame", wx.OK | wx.ICON_ERROR | wx.CENTRE)

    def autoNext(self):
        headers = self.__stream.getHeaders()
        if headers['current_frame'] == headers['nframes']:
            self.goToFirst()
        else:
            self.goToNext()

    def goToLast(self):
        try:
            self.__stream.last()
            self.__frame = self.__stream.read()
            self.drawFrame()
        except Exception as err:
            wx.MessageBox(str(err), "Moving to the last frame", wx.OK + wx.ICON_ERROR | wx.CENTRE)

    def goToCallback(self):
        from .GoToWindow import GoToWindow
        dlg = GoToWindow(self, self.__stream)
        if dlg.ShowModal() == wx.ID_OK:
            n = dlg.getFrameNumber()
            self.goTo(n)

    def goTo(self, n):
        self.__stream.first()
        self.__stream.move(n)
        self.__frame = self.__stream.read()
        self.drawFrame()

    def saveFrameBin(self, filename):
        if filename[-4:] != ".bin":
            filename += ".bin"
        from vis_brain.readers import BinReader
        reader = BinReader(filename)
        reader.setHeightUm(self.__stream.getHeaders()["height_um"])
        reader.setWidthUm(self.__stream.getHeaders()["width_um"])
        reader.save(self.__frame)

    def saveFrameMat(self, filename):
        if filename[-4:] != ".mat":
            filename += ".mat"
        import scipy.io
        output_data = {
            "stream_name": self.__filename,
            "frame_number": self.__stream.getHeaders()["current_frame"],
            "frame_time":
                (self.__stream.getHeaders()["current_frame"] - 1) * 1000 / self.__stream.getHeaders()['sample_rate'],
            "frame_time_units": "ms",
            "height": self.__stream.getHeaders()['height'],
            "width": self.__stream.getHeaders()["width"],
            "height_um": self.__stream.getHeaders()["height_um"],
            "width_um": self.__stream.getHeaders()["width_um"],
            "data": self.__frame
        }
        scipy.io.savemat(filename, output_data)

    def saveFramePng(self, filename):
        print("SAVE FRAME TO PNG: " + filename)

    def saveFrameCallback(self):
        dlg = wx.FileDialog(self, "Save Frame", wildcard="vis-brain data file (.bin)|*.bin|" +
                            "MATLAB/Octave file (.mat)|*.mat|Image (.png)|*.png",
                            style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT)
        if dlg.ShowModal() == wx.ID_OK:
            filename = dlg.GetPath()
            index = dlg.GetFilterIndex()
            if index == 0:
                self.saveFrameBin(filename)
            if index == 1:
                self.saveFrameMat(filename)
            if index == 2:
                self.saveFramePng(filename)

    def play(self):
        actualTime = 1000 / self.__stream.getHeaders()['sample_rate']
        presentationTime = int(actualTime * 20)
        self.__timer.Start(presentationTime, wx.TIMER_CONTINUOUS)

    def stop(self):
        self.__timer.Stop()

    def jet(self):
        self.drawFrame()

    def hsv(self):
        self.drawFrame()

    def gray(self):
        self.drawFrame()

    def colorbarCallback(self, event):
        self.drawFrame()

    def setLimits(self, minValue, maxValue):
        self.__clim = minValue, maxValue
        self.drawFrame()

    def setLimitsCallback(self):
        from .SetLimitsWindow import SetLimitsWindow
        dlg = SetLimitsWindow(self, self.__clim)
        if dlg.ShowModal() == wx.ID_OK:
            limits = dlg.getLimits()
            self.setLimits(limits[0], limits[1])