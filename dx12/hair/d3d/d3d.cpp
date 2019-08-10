#include "stdafx.h"
#include "d3d.h"


namespace win32::d3d
{
	core::pixelformat D3DFormat2PixelFormat(D3DFORMAT d3dformat)
	{
		switch (d3dformat)
		{
		case D3DFMT_A8R8G8B8: return core::pixelformat::argb;
		case D3DFMT_A8B8G8R8: return core::pixelformat::abgr;
		case D3DFMT_X8R8G8B8: return core::pixelformat::xrgb;
		case D3DFMT_X8B8G8R8: return core::pixelformat::xbgr;
		case D3DFMT_YUY2: return core::pixelformat::yuy2;
		case D3DFMT_UYVY: return core::pixelformat::uyvy;

		case D3DFormatExNV12: return core::pixelformat::nv12;
		case D3DFormatExYV12: return core::pixelformat::yv12;
		case D3DFormatEx411P: return core::pixelformat::yuv2;
		case D3DFormatExP010: return core::pixelformat::p010;
		case D3DFormatExRGBP: return core::pixelformat::rgb;
		case D3DFormatExBGRP: return core::pixelformat::bgr;
		default: return core::pixelformat::none;
		}
	}

	D3DFORMAT PixelFormat2D3DFormat(core::pixelformat pixelformat)
	{
		switch (pixelformat)
		{
		case core::pixelformat::argb: return D3DFMT_A8R8G8B8;
		case core::pixelformat::abgr: return D3DFMT_A8B8G8R8;
		case core::pixelformat::xrgb: return D3DFMT_X8R8G8B8;
		case core::pixelformat::xbgr: return D3DFMT_X8B8G8R8;
		case core::pixelformat::nv12: return (D3DFORMAT)D3DFormatExNV12;
		case core::pixelformat::p010: return (D3DFORMAT)D3DFormatExP010;
		case core::pixelformat::yv12: return (D3DFORMAT)D3DFormatExYV12;
		default: return D3DFMT_UNKNOWN;
		}
	}

	core::pixelformat DXGIFormat2PixelFormat(DXGI_FORMAT dxgiformat)
	{
		switch (dxgiformat)
		{
		case DXGI_FORMAT_B8G8R8X8_UNORM: return core::pixelformat::bgrx;
		case DXGI_FORMAT_B8G8R8A8_UNORM: return core::pixelformat::bgra;
		case DXGI_FORMAT_NV12: return core::pixelformat::nv12;
		case DXGI_FORMAT_YUY2: return core::pixelformat::yuy2;
		case DXGI_FORMAT_P010: return core::pixelformat::p010;
		case D3DFormatExRGBP: return core::pixelformat::rgb;
		case D3DFormatExBGRP: return core::pixelformat::bgr;
		case DXGI_FORMAT_420_OPAQUE: return core::pixelformat::yuy2;
		default: return core::pixelformat::none;
		}
	}

	DXGI_FORMAT PixelFormat2DXGIFormat(core::pixelformat pixelformat)
	{
		switch (pixelformat)
		{
		case core::pixelformat::bgrx: return DXGI_FORMAT_B8G8R8X8_UNORM;
		case core::pixelformat::bgra: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case core::pixelformat::nv12: return DXGI_FORMAT_NV12;
		case core::pixelformat::yuy2: return DXGI_FORMAT_420_OPAQUE;
		case core::pixelformat::p010: return DXGI_FORMAT_P010;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

#define DXGI_FORMAT_RETURN_NAME(fmt) case fmt: return #fmt

	const char * D3DFormatName(_D3DFORMAT d3dformat)
	{
		switch (d3dformat)
		{
			DXGI_FORMAT_RETURN_NAME(D3DFMT_UNKNOWN);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_R8G8B8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A8R8G8B8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_X8R8G8B8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_R5G6B5);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_X1R5G5B5);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A1R5G5B5);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A4R4G4B4);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_R3G3B2);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A8R3G3B2);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_X4R4G4B4);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A2B10G10R10);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A8B8G8R8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_X8B8G8R8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_G16R16);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A2R10G10B10);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A16B16G16R16);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_A8P8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_P8);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_L8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A8L8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A4L4);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_V8U8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_L6V5U5);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_X8L8V8U8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_Q8W8V8U8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_V16U16);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A2W10V10U10);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_UYVY);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_R8G8_B8G8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_YUY2);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_G8R8_G8B8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_DXT1);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_DXT2);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_DXT3);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_DXT4);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_DXT5);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_D16_LOCKABLE);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D32);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D15S1);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D24S8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D24X8);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D24X4S4);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D16);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_D32F_LOCKABLE);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D24FS8);

#if !defined(D3D_DISABLE_9EX)
			DXGI_FORMAT_RETURN_NAME(D3DFMT_D32_LOCKABLE);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_S8_LOCKABLE);
#endif

			DXGI_FORMAT_RETURN_NAME(D3DFMT_L16);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_VERTEXDATA);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_INDEX16);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_INDEX32);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_Q16W16V16U16);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_MULTI2_ARGB8);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_R16F);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_G16R16F);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A16B16G16R16F);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_R32F);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_G32R32F);
			DXGI_FORMAT_RETURN_NAME(D3DFMT_A32B32G32R32F);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_CxV8U8);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_A1);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_A2B10G10R10_XR_BIAS);

			DXGI_FORMAT_RETURN_NAME(D3DFMT_BINARYBUFFER);

			DXGI_FORMAT_RETURN_NAME(D3DFormatExNV12);
			DXGI_FORMAT_RETURN_NAME(D3DFormatExYV12);
			DXGI_FORMAT_RETURN_NAME(D3DFormatExP010);
			DXGI_FORMAT_RETURN_NAME(D3DFormatExIYUV);
			DXGI_FORMAT_RETURN_NAME(D3DFormatEx411P);
			DXGI_FORMAT_RETURN_NAME(D3DFormatEx444P);
			DXGI_FORMAT_RETURN_NAME(D3DFormatExRGBP);
			DXGI_FORMAT_RETURN_NAME(D3DFormatExBGRP);


		default: return "Invalid D3D Format";
		}

	}

	const char * DXGIFormatName(DXGI_FORMAT dxgiformat)
	{
		switch (dxgiformat)
		{
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_UNKNOWN);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G8X24_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_X32_TYPELESS_G8X24_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10A2_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10A2_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10A2_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R11G11B10_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D32_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R24G8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D24_UNORM_S8_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_X24_TYPELESS_G8_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_FLOAT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D16_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_UINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_SINT);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_A8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R1_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R9G9B9E5_SHAREDEXP);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_B8G8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_G8R8_G8B8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC1_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC1_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC1_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC2_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC2_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC2_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC3_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC3_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC3_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC4_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC4_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC4_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC5_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC5_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC5_SNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B5G6R5_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B5G5R5A1_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8A8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8X8_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8A8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8X8_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC6H_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC6H_UF16);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC6H_SF16);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC7_TYPELESS);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC7_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC7_UNORM_SRGB);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_AYUV);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y410);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y416);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_NV12);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P010);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P016);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_420_OPAQUE);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_YUY2);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y210);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y216);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_NV11);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_AI44);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_IA44);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P8);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_A8P8);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B4G4R4A4_UNORM);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P208);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_V208);
			DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_V408);
		default: return "Invalid DXGI Format";
		}

	}
}
