//
// Created by serik1987 on 12.10.2019.
//

#include "PngReader.h"
#include "../ContiguousMatrix.h"

namespace data::reader {

    void PngReader::write(data::Matrix &matrix) {
        write_open_file(filename.c_str());
        write_initialize_png_handler();
        write_initialize_png_info_handler();
        write_error_handling();
        write_init_io();
        write_set_header();
        write_png_header();
        switch (colorAxis.getColormap()){
            case data::graph::ColorAxis::GrayColormap:
                write_create_buffer_gray();
                break;
            case data::graph::ColorAxis::JetColormap:
            case data::graph::ColorAxis::HsvColormap:
                write_create_buffer_color();
                break;
            default:
                throw png_exception();
        }
        write_put_buffer();
        write_destroy_all_buffers();
    }

    inline void PngReader::write_open_file(const char* filename){
        fp = fopen(filename, "wb");
        if (!fp){
            fp = nullptr;
            write_destroy_all_buffers();
            sys::create_exception();
        }
    }

    inline void PngReader::write_initialize_png_handler() {
        png_handler = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_handler){
            png_handler = nullptr;
            write_destroy_all_buffers();
            throw png_exception();
        }
    }

    inline void PngReader::write_initialize_png_info_handler() {
        png_info_handler = png_create_info_struct(png_handler);
        if (!png_info_handler){
            png_info_handler = nullptr;
            write_destroy_all_buffers();
            throw png_exception();
        }
    }

    inline void PngReader::write_init_io(){
        png_init_io(png_handler, fp);
        write_error_handling();
    }

    inline void PngReader::write_set_header() {
        int color_type;
        if (getColorAxis().getColormap() == data::graph::ColorAxis::GrayColormap){
            color_type = PNG_COLOR_TYPE_GRAY;
        } else {
            color_type = PNG_COLOR_TYPE_RGB;
        }

        png_set_IHDR(
                png_handler,            // handle to the PNG file
                png_info_handler,       // Handler to the information about PNG file
                buffer->getWidth(),     // Image width in pixels
                buffer->getHeight(),    // Image height in px
                16,                     // Bit depth, let's take maximum depth
                color_type,             // Color type
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
                );

        png_set_pHYs(
                png_handler,
                png_info_handler,
                (png_uint_32)(buffer->getWidth()/buffer->getWidthUm()),
                (png_uint_32)(buffer->getHeight()/buffer->getHeightUm()),
                PNG_RESOLUTION_UNKNOWN
                );
    }

    inline void PngReader::write_png_header(){
        png_write_info(png_handler, png_info_handler);
        write_error_handling();
    }

    inline void PngReader::write_create_buffer_gray(){
        graphic_buffer = new png_bytep[buffer->getHeight()];
        for (int y = 0; y < buffer->getHeight(); ++y){
            graphic_buffer[y] = new png_byte[2 * buffer->getWidth()];
            for (int x = 0; x < buffer->getWidth(); ++x){
                double original_value = buffer->getValue(y, x);
                double gray_scale = colorAxis.getGrayPixel(original_value);
                int gray_scale_int = (int)(gray_scale * 65535);
                graphic_buffer[y][2*x] = gray_scale_int / 256;
                graphic_buffer[y][2*x + 1] = gray_scale_int % 256;
            }
        }
    }

    static const int RH = 0;
    static const int RL = 1;
    static const int GH = 2;
    static const int GL = 3;
    static const int BH = 4;
    static const int BL = 5;

    inline void PngReader::write_create_buffer_color(){
        graphic_buffer = new png_bytep[buffer->getHeight()];
        for (int y = 0; y < buffer->getHeight(); ++y){
            graphic_buffer[y] = new png_byte[6 * buffer->getWidth()];
            for (int x = 0; x < buffer->getWidth(); ++x){
                png_bytep pixel = graphic_buffer[y] + 6 * x;
                double value = buffer->getValue(y, x);
                data::graph::ColorAxis::ColorPixel pixel_value = colorAxis.getColorPixel(value);
                int red = (int)(pixel_value.red * 65535);
                int green = (int)(pixel_value.green * 65535);
                int blue = (int)(pixel_value.blue * 65535);
                pixel[RH] = red / 256;
                pixel[RL] = red % 256;
                pixel[GH] = green / 256;
                pixel[GL] = green % 256;
                pixel[BH] = blue / 256;
                pixel[BL] = blue % 256;
            }
        }
    }

    inline void PngReader::write_put_buffer(){
        if (graphic_buffer != nullptr) {
            png_write_image(png_handler, graphic_buffer);
            write_error_handling();
            png_write_end(png_handler, NULL);
        }
    }

    void PngReader::write_error_handling() {
        if (setjmp(png_jmpbuf(png_handler))){
            write_destroy_all_buffers();
            throw png_exception();
        }
    }

    void PngReader::write_destroy_all_buffers() {
        if (graphic_buffer != nullptr){
            for (int y = 0; y < buffer->getHeight(); ++y){
                delete [] graphic_buffer[y];
            }
            delete [] graphic_buffer;
            graphic_buffer = nullptr;
        }
        if (png_handler != nullptr && png_info_handler != nullptr){
            png_destroy_write_struct(&png_handler, &png_info_handler);
            png_handler = nullptr;
            png_info_handler = nullptr;
        } else if (png_handler != nullptr && png_info_handler == nullptr){
            png_destroy_write_struct(&png_handler, NULL);
            png_handler = nullptr;
        }
        if (fp != nullptr){
            fclose(fp);
            fp = nullptr;
        }
    }

}