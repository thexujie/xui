#include "stdafx.h"
#include "jpg.h"

#include <setjmp.h>
#include <stdio.h>
#include "../deps/libjpeg/jpeglib.h"
#pragma comment(lib, "libjpeg.lib")

namespace graphics::image::formats
{
    using namespace core;

    struct irr_jpeg_error_mgr
    {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
    };


    void init_source(j_decompress_ptr cinfo) { }

    boolean fill_input_buffer(j_decompress_ptr cinfo)
    {
        return true;
    }

    void skip_input_data(j_decompress_ptr cinfo, long count)
    {
        jpeg_source_mgr * src = cinfo->src;
        if (count > 0)
        {
            src->bytes_in_buffer -= count;
            src->next_input_byte += count;
        }
    }

    void term_source(j_decompress_ptr cinfo) { }

    void error_exit(j_common_ptr cinfo)
    {
        (*cinfo->err->output_message)(cinfo);

        irr_jpeg_error_mgr * myerr = (irr_jpeg_error_mgr*)cinfo->err;

        longjmp(myerr->setjmp_buffer, 1);
    }


    void output_message(j_common_ptr cinfo)
    {
        char temp1[JMSG_LENGTH_MAX];
        (*cinfo->err->format_message)(cinfo, temp1);
    }

    // 做 10 个字节的测试
    bool is_jpg_data(const byte_t * buffer, int32_t length)
    {
        if (length < 11)
            return false;

        for (int32_t cnt = 0; cnt < sizeof(JPEG_HEADER); ++cnt)
        {
            if ((uint8_t)buffer[cnt] != JPEG_HEADER[cnt])
                return false;
        }

        //for(int cnt = 0; cnt != 2; ++cnt)
        //{
        //	if(buffer[cnt + length - 2] != JPEG_TAIL[cnt])
        //		return false;
        //}
        return true;
    }

    // 创建一幅JPEG图片的字节数组
    core::error jpg_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image)
    {
        byte_t ** rows = nullptr;
        // 解压信息
        struct jpeg_decompress_struct cinfo;
        // 错误报告
        struct irr_jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr.pub);
        cinfo.err->error_exit = error_exit;
        cinfo.err->output_message = output_message;

        if (setjmp(jerr.setjmp_buffer))
        {
            jpeg_destroy_decompress(&cinfo);
            image_free(rows);
        }

        jpeg_create_decompress(&cinfo);

        jpeg_source_mgr jsrc;

        jsrc.bytes_in_buffer = length;
        jsrc.next_input_byte = (const JOCTET *)buffer;
        cinfo.src = &jsrc;

        jsrc.init_source = init_source;
        jsrc.fill_input_buffer = fill_input_buffer;
        jsrc.skip_input_data = skip_input_data;
        jsrc.term_source = term_source;
        jsrc.resync_to_restart = jpeg_resync_to_restart;

        jpeg_read_header(&cinfo, TRUE);

        cinfo.out_color_space = JCS_RGB;
        cinfo.out_color_components = 3;
        cinfo.do_fancy_upsampling = FALSE;

        jpeg_start_decompress(&cinfo);

        image_format format = {};
        format.format = format_b8g8r8;
        format.width = (int32_t)cinfo.image_width;
        format.height = (int32_t)cinfo.image_height;

        int32_t src_pitch = format.width * 3;
        byte_t * src_buffer = image_malloc(src_pitch * format.height);

        rows = (byte_t **)image_malloc(sizeof(byte_t *) * format.height);
        for (int32_t cnt = 0; cnt < format.height; ++cnt)
            rows[cnt] = src_buffer + cnt * src_pitch;

        uint32_t rowsRead = 0;
        // 每次读取一行
        while (cinfo.output_scanline < cinfo.output_height)
            rowsRead += jpeg_read_scanlines(&cinfo, (JSAMPLE **)&rows[rowsRead], cinfo.output_height - rowsRead);

        image_free(rows);
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        image.format = format;
        if (ictx.get_format)
            image.format = ictx.get_format(image_type_jpeg, format);

        ictx.pfn_alloc(image, 4);
        image.pfn_free = ictx.pfn_free;

        image_data_t src_data = {};
        src_data.format = format;
        src_data.data = src_buffer;
        src_data.pitch = src_pitch;

        error err = image_convert_ex(ictx, src_data, image);
        if (err < 0)
        {
            image.pfn_free(image);
            return err;
        }
        return error_ok;
    }

    bool jpg_rule_default(image_codec_context * rule)
    {
        return false;
    }
}
