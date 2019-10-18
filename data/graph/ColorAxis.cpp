//
// Created by serik1987 on 18.10.2019.
//

#include <cmath>
#include "ColorAxis.h"

namespace data::graph {

    ColorAxis::ColorPixel ColorAxis::getColorPixel_jet(double value) {
        ColorPixel pixel;

        if (value < cmin) value = cmin;
        if (value > cmax) value = cmax;
        pixel.red = (value - cmin) / (cmax - cmin);
        pixel.green = 1 - abs(value - cp) / (cmax - cp);
        pixel.blue = (cmax - value) / (cmax - cmin);

        return pixel;
    }

    ColorAxis::ColorPixel ColorAxis::getColorPixel_hsv(double value) {
        ColorPixel pixel = {0.0, 0.0, 0.0};
        double hue;
        int ihue;
        int hue_category;
        double a, Vinc, Vdec;

        if (value < cmin) value = cmin;
        if (value > cmax) value = cmax;
        hue = (value - cmin) * 360 / (cmax - cmin);
        ihue = (int)hue;
        hue_category = ihue / 60 % 6;
        a = (double)(ihue % 60) / 60;
        Vinc = a;   Vdec = 1.0 - a;

        switch (hue_category){
            case 0:
                pixel.red = 1.0;
                pixel.green = Vinc;
                pixel.blue = 0.0;
                break;
            case 1:
                pixel.red = Vdec;
                pixel.green = 1.0;
                pixel.blue = 0.0;
                break;
            case 2:
                pixel.red = 0.0;
                pixel.green = 1.0;
                pixel.blue = Vinc;
                break;
            case 3:
                pixel.red = 0.0;
                pixel.green = Vdec;
                pixel.blue = 1.0;
                break;
            case 4:
                pixel.red = Vinc;
                pixel.green = 0.0;
                pixel.blue = 1.0;
                break;
            case 5:
                pixel.red = 1.0;
                pixel.green = 0.0;
                pixel.blue = Vdec;
        }


        return pixel;
    }

}