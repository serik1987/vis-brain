#!/usr/bin/env python
#-*- coding: utf-8

import numpy as np
import matplotlib.pyplot as plt
import os
from struct import unpack

if __name__ == "__main__":
    os.chdir("cmake-build-debug")
    f = open("sample.dat", "rb")
    cols = 1000
    rows = 1000
    items = cols * rows
    raw_data = f.read(8*items)
    data = np.array(unpack("d"*items, raw_data))
    data = data.reshape(1000, 1000)
    f.close()
    plt.imshow(data)
    plt.set_cmap("jet")
    plt.colorbar();
    plt.show()