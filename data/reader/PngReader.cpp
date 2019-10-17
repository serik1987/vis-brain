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
        switch (colormap){
            case GrayColormap:
                write_create_buffer_gray();
                break;
            case JetColormap:
                write_create_buffer_jet();
                break;
            case HsvColormap:
                write_create_buffer_hsv();
                break;
            default:
                std::cout << "BUFFER NOT CREATED\n";
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
        if (colormap == GrayColormap){
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
                double gray_scale = (original_value - minValue) / (maxValue - minValue);
                int gray_scale_int = (int)(gray_scale * 65536);
                if (gray_scale_int > 65535) gray_scale_int = 65535;
                if (gray_scale_int < 0) gray_scale_int = 0;
                graphic_buffer[y][2*x] = gray_scale_int / 256;
                graphic_buffer[y][2*x + 1] = gray_scale_int % 256;
            }
        }
    }

    const int RH = 0;
    const int RL = 1;
    const int GH = 2;
    const int GL = 3;
    const int BH = 4;
    const int BL = 5;

    inline void PngReader::write_create_buffer_jet(){
        graphic_buffer = new png_bytep[buffer->getHeight()];
        double cp = 0.5 * (minValue + maxValue);
        for (int y = 0; y < buffer->getHeight(); ++y){
            graphic_buffer[y] = new png_byte[6 * buffer->getWidth()];
            for (int x = 0; x < buffer->getWidth(); ++x){
                png_bytep pixel = graphic_buffer[y] + 6 * x;
                double value = buffer->getValue(y, x);
                if (value < minValue) value = minValue;
                if (value > maxValue) value = maxValue;
                double redChannel = (value - minValue) / (maxValue - minValue);
                double greenChannel = 1 - abs(value - cp) / (maxValue - cp);
                double blueChannel = (maxValue - value) / (maxValue - minValue);
                int red = (int)(redChannel * 65535);
                int green = (int)(greenChannel * 65535);
                int blue = (int)(blueChannel * 65535);
                pixel[RH] = red / 256;
                pixel[RL] = red % 256;
                pixel[GH] = green / 256;
                pixel[GL] = green % 256;
                pixel[BH] = blue / 256;
                pixel[BL] = blue % 256;
            }
        }
    }

    inline void PngReader::write_create_buffer_hsv() {
        graphic_buffer = new png_bytep[buffer->getHeight()];
        for (int y = 0; y < buffer->getHeight(); ++y){
            png_bytep pixel = graphic_buffer[y] = new png_byte[6 * buffer->getWidth()];
            for (int x = 0; x < buffer->getWidth(); ++x, pixel += 6){
                double value = buffer->getValue(y, x);
                if (value < minValue) value = minValue;
                if (value > maxValue) value = maxValue;
                double hueValue = (value - minValue) * 360 / (maxValue - minValue);
                int hue = (int)hueValue;
                int hue_category = hue / 60 % 6;
                int a = 65535 * (hue % 60) / 60;
                int Vinc = a;
                int Vdec = 65535 - a;
                int red = 0;
                int green = 0;
                int blue = 0;
                switch (hue_category){
                    case 0:
                        red = 65535;
                        green = Vinc;
                        blue = 0;
                        break;
                    case 1:
                        red = Vdec;
                        green = 65535;
                        blue = 0;
                        break;
                    case 2:
                        red = 0;
                        green = 65535;
                        blue = Vinc;
                        break;
                    case 3:
                        red = 0;
                        green = Vdec;
                        blue = 65535;
                        break;
                    case 4:
                        red = Vinc;
                        green = 0;
                        blue = 65535;
                        break;
                    case 5:
                        red = 65535;
                        green = 0;
                        blue = Vdec;
                }
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