//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_PNGREADER_H
#define MPI2_PNGREADER_H

#include <png.h>
#include "ExternalSaver.h"
#include "../graph/ColorAxis.h"

namespace data::reader {

    class PngReader: public ExternalSaver {

    private:
        data::graph::ColorAxis colorAxis;

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
        inline void write_create_buffer_color();
        inline void write_put_buffer();
        inline void write_error_handling();
        void write_destroy_all_buffers();

    protected:
        void write(data::Matrix& matrix) override;

    public:
        PngReader(const std::string& filename): ExternalSaver(filename), Saver(filename), Reader(filename),
            colorAxis(data::graph::ColorAxis::GrayColormap, 0.0, 1.0){};
        ~PngReader() override = default;

        /**
         * Returns the color axis. This colormap will be used during the consequtive savings
         *
         * @return current colormap
         */
        data::graph::ColorAxis& getColorAxis() { return colorAxis; }

        class png_exception: public std::exception{
        public:
            const char* what() const noexcept override{
                return "An error occured during reading/writing to PNG file";
            }
        };
    };

}


#endif //MPI2_PNGREADER_H
