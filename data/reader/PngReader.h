//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_PNGREADER_H
#define MPI2_PNGREADER_H

#include <png.h>
#include "ExternalSaver.h"

namespace data::reader {

    class PngReader: public ExternalSaver {
    public:
        enum Colormap {GrayColormap, JetColormap, HsvColormap};

    private:
        Colormap colormap = GrayColormap;
        double minValue = 0.0, maxValue = 1.0;

        /* Some service variables for the writing process */
        FILE* fp = nullptr;
        png_structp png_handler = nullptr;
        png_infop png_info_handler = nullptr;
        png_bytepp graphic_buffer = nullptr;

        /* Facades or low-level routines */
        inline void write_open_file(const char* filename);
        inline void write_initialize_png_handler();
        inline void write_initialize_png_info_handler();
        inline void write_init_io();
        inline void write_set_header();
        inline void write_png_header();
        inline void write_create_buffer_gray();
        inline void write_create_buffer_jet();
        inline void write_create_buffer_hsv();
        inline void write_put_buffer();
        inline void write_error_handling();
        void write_destroy_all_buffers();

    protected:
        void write(data::Matrix& matrix) override;

    public:
        PngReader(const std::string& filename): ExternalSaver(filename), Saver(filename), Reader(filename) {};
        ~PngReader() override = default;

        /**
         * Returns a current colormap. This colormap will be used during the consequtive savings
         *
         * @return current colormap
         */
        Colormap getColormap() { return colormap; }

        /**
         * Sets the new colormap. You may set one of the following colormaps:
         * data::reader::PngReader::GrayColormap black-and-white plot
         * data::reader::PngReader::JetColormap colored colormap, for non-periodic values
         * data::reader::PngReader::HsvColormap colored colormap, for periodic values
         *
         * @param new_colormap new colormap to set
         */
        void setColormap(Colormap new_colormap) { colormap = new_colormap; }

        /**
         * Returns minimum value for the color axis (0.0 by default)
         *
         * @return the minimum value
         */
        double getMinValue() { return minValue; }

        /**
         * Returns maximum value for the color axis (1.0 by default
         *
         * @return
         */
        double getMaxValue() { return maxValue; }

        /**
         * Sets the range for the color axis
         *
         * @param min minimum value within the range. All values below it will be replaced to this value
         * @param max maximum value within the range. All values above it will be replaced to this value
         */
        void setColorRange(double min, double max) { minValue = min, maxValue = max; }

        class png_exception: public std::exception{
        public:
            const char* what() const noexcept override{
                return "An error occured during reading/writing to PNG file";
            }
        };
    };

}


#endif //MPI2_PNGREADER_H
