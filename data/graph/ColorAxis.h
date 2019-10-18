//
// Created by serik1987 on 18.10.2019.
//

#ifndef MPI2_COLORAXIS_H
#define MPI2_COLORAXIS_H

namespace data::graph {

    /**
     * Color axis is an important component of data::reader::PngReader and data::streamer::AviStreamer that is
     * included to these instances as a child object. In order to reveal instance of this object use getColorAxis()
     * method (this is the same for both classes
     */
    class ColorAxis {
    public:
        enum Colormap {GrayColormap, JetColormap, HsvColormap};
        typedef double GrayPixel;
        struct ColorPixel{
            double red;
            double green;
            double blue;
        };

    private:
        double cmin, cmax, cp;
        Colormap colormap;

        ColorPixel getColorPixel_jet(double value);
        ColorPixel getColorPixel_hsv(double value);

    public:
        ColorAxis(Colormap colormap, double cmin, double cmax): colormap(colormap), cmin(cmin), cmax(cmax) {};
        ColorAxis(): colormap(GrayColormap), cmin(0.0), cmax(1.0) {};
        virtual ~ColorAxis() {};

        /**
         * Sets the colormap. You may use one of the following color maps:
         * data::graph::ColorAxis::GrayColormap - black-and-white colors on the graph
         * data::graph::ColorAxis::
         *
         * @param colormap new colormap to set
         */
        void setColormap(Colormap colormap) {this->colormap = colormap; }

        /**
         * Returns minimum value for the color. All other values will be transformed to this minimum value
         *
         * @return the minimum value (double)
         */
        double getMinValue() { return cmin;}

        /**
         * Returns maximum value for the color. All other values will be transformed to this maximum value
         *
         * @return the maximum value
         */
        double getMaxValue() { return cmax;}

        /**
         * Returns an appropriate colormap. Colormap is how certain value are coded by colors
         *
         * @return the colormap (simple enumeration)
         */
        Colormap getColormap() { return colormap;}

        /**
         * Sets the limits on the color axis
         *
         * @param cmin minimuum value to show. All values below this minimum will be rounded to it
         * @param cmax maximum value to show. All values above this maximum will be rounded to it
         */
        void setColorRange(double cmin, double cmax) {
            this->cmin = cmin;
            this->cmax = cmax;
            cp = 0.5 * (cmin + cmax);
        }

        /**
         * Transforms matrix value to the lightness level.
         * Lightness level = 0.0 corresponds to the minimum value while lightness level of 1.0 corresponds to
         * the maximum value
         *
         * @param value source value of the matrix item
         * @return the lighness level
         */
        GrayPixel getGrayPixel(double value){
            if (value > cmax) value = cmax;
            if (value < cmin) value = cmin;
            return (value - cmin) / (cmax - cmin);
        };

        /**
         * Returns the value of the color pixel corresponds to the certain matrix item
         *
         * @param value value of this matrix item
         * @return structure with the following field: red - value of the red channel in [0.0, 1.0] range;
         * green - value of the green channel in [0.0, 1.0] range, blue - value of the blue channel in [0.0, 1.0]
         * range
         */
        ColorPixel getColorPixel(double value){
            switch (colormap){
                case GrayColormap: return {getGrayPixel(value), getGrayPixel(value), getGrayPixel(value)};
                case JetColormap: return getColorPixel_jet(value);
                case HsvColormap: return getColorPixel_hsv(value);
            }
        }
    };

}


#endif //MPI2_COLORAXIS_H
