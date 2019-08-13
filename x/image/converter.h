#pragma once

#include "format.h"

namespace core
{
	/**
	* ����ͼ��dst �� src ������ͬ����ɫ��ʽ��
	* Ŀ��ͼ��� width �� height ����С�� Դͼ��
	*/
	core::error image_convert_copy_ex(image_codec_context & icctx, const image_data_t & src, const image_data_t & dst);

	/**
	* image_convert_ex ֻ�ܽ�һ�� rgb ��ʽת������һ�� rgb ��ʽ��
	* ���ܽ��е�ɫ�塢ѹ���Ȳ�����
	*/
	core::error image_convert_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
	core::error image_convert_index1_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
	core::error image_convert_index2_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
	core::error image_convert_index4_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
	core::error image_convert_index8_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);


	// ������ɫ���������ɫ����
	format format_from_mask_abgr(const color_mask_abgr_t & mask, int32_t bits);
	color_mask_abgr_t mask_from_format_abgr(format format);


	//////////////////////////////////////////////////////////////////////////
	/// ��������֧����ɫ֮����໥ת��

	// - ����
	void color_8_to_8(const void * src_pixel, void * dst_pixel);
	void color_16_to_16(const void * src_pixel, void * dst_pixel);
	void color_24_to_24(const void * src_pixel, void * dst_pixel);
	void color_32_to_32(const void * src_pixel, void * dst_pixel);
	void color_32_to_32_ex0(const void * src_pixel, void * dst_pixel);

	void color_r3g3b2_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_r3g3b2_to_a1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_r3g3b2_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r3g3b2_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_a8r3g3b2_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a8r3g3b2_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);

	void color_r8g8b8_to_b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_r8g8b8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r8g8b8_to_r8g8b8a8(const void * src_pixel, void * dst_pixel);
	void color_a8r8g8b8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a8b8g8r8_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_x8b8g8r8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_x8r8g8b8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_x8r8g8b8_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_x8r8g8b8_to_r5g6b5(const void * src_pixel, void * dst_pixel);

	void color_r5g6b5_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_r5g6b5_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r5g6b5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r5g6b5_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_b5g6r5_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_b5g6r5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);


	void color_a1r5g5b5_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_a1r5g5b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a1r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a4r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_x4r4g4b4_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_x4r4g4b4_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_x4r4g4b4_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_a4r4g4b4_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a4r4g4b4_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_a4r4g4b4_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);

	void color_a16b16g16r16_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a16b16g16r16f_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_g16r16_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_g16r16f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r16f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r32f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_g32r32f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a32b32g32r32f_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_gray8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_gray8_to_r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_x1r5g5b5_to_a1r5g5b5(const void * src_pixel, void * dst_pixel);
	void color_x1r5g5b5_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_x1r5g5b5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_x1r5g5b5_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);

	void color_a8r8g8b8_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_yuv_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r8g8b8_to_yuv(const void * src_pixel, void * dst_pixel);
	void color_yuv_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_ycck_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_ycck_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_gray_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_gray_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_cmyk_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_cmyk_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_r8g8b8_to_x32y32z32f(const void * src_pixel, void * dst_pixel);
	void color_x32y32z32f_tor_r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_r8g8b8x8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_r8g8b8x8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_r8g8b8a8_to_b8g8r8a8(const void * src_pixel, void * dst_pixel);
	void color_a8r8g8b8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);


	void color_r8g8b8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_r8g8b8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_b8g8r8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_b8g8r8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_x8r8g8b8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
	void color_x8b8g8r8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);


	void color_a2r10g10b10_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

	void color_a2b10g10r10_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
	void color_a2b10g10r10_to_a2b10g10r10(const void * src_pixel, void * dst_pixel);
	void color_a2b10g10r10_to_a2r10g10b10(const void * src_pixel, void * dst_pixel);

	//
	uint8_t color_float16_to_8(uint16_t f16);
}

