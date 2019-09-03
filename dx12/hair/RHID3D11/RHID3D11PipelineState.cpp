#include "stdafx.h"
#include "RHID3D11PipelineState.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11PipelineState::Create(const PipelineStateArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		assert(device);

		if (args.CS.empty())
			return _CreateGraphicsPipelineState(args);
		else
			return _CreateComputePipelineState(args);
	}

	void RHID3D11PipelineState::SetName(const std::u8string & name)
	{
		//SetD3D12ObjectName(_pipelineState.get(), name + u8"._pipelineState");
		//SetD3D12ObjectName(_rootSignature.get(), name + u8"._rootSignature");
	}
	
	core::error RHID3D11PipelineState::_CreateGraphicsPipelineState(const PipelineStateArgs & args)
	{
		HRESULT hr = S_OK;

		auto device = _device->Device();
		assert(device);
		
		UINT compileFlags = 0;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		core::comptr<ID3DBlob> errorMessage;

		core::comptr<ID3DBlob> vertexShaderCode;
		core::comptr<ID3D11VertexShader> vertexShader;
		if (!args.VS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.VS).c_str(), nullptr, nullptr, args.VSMain.c_str(), "vs_5_0", compileFlags, 0, vertexShaderCode.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<VS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}

			hr = device->CreateVertexShader(vertexShaderCode->GetBufferPointer(), vertexShaderCode->GetBufferSize(), nullptr, vertexShader.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateVertexShader failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> hullShaderCode;
		core::comptr<ID3D11HullShader> hullShader;
		if (!args.HS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.HS).c_str(), nullptr, nullptr, args.HSMain.c_str(), "hs_5_0", compileFlags, 0, hullShaderCode.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<HS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}

			hr = device->CreateHullShader(hullShaderCode->GetBufferPointer(), hullShaderCode->GetBufferSize(), nullptr, hullShader.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateHullShader failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> domainShaderCode;
		core::comptr<ID3D11DomainShader> domainShader;
		if (!args.DS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.DS).c_str(), nullptr, nullptr, args.DSMain.c_str(), "ds_5_0", compileFlags, 0, domainShaderCode.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<DS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}

			hr = device->CreateDomainShader(domainShaderCode->GetBufferPointer(), domainShaderCode->GetBufferSize(), nullptr, domainShader.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateDomainShader failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> geometryShaderCode;
		core::comptr<ID3D11GeometryShader> geometryShader;
		if (!args.GS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.GS).c_str(), nullptr, nullptr, args.GSMain.c_str(), "gs_5_0", compileFlags, 0, geometryShaderCode.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<GS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}

			hr = device->CreateGeometryShader(geometryShaderCode->GetBufferPointer(), geometryShaderCode->GetBufferSize(), nullptr, geometryShader.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateDomainShader failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> pixelShaderCode;
		core::comptr<ID3D11PixelShader> pixelShader;
		if (!args.PS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.PS).c_str(), nullptr, nullptr, args.PSMain.c_str(), "ps_5_0", compileFlags, 0, pixelShaderCode.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<PS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}

			hr = device->CreatePixelShader(pixelShaderCode->GetBufferPointer(), pixelShaderCode->GetBufferSize(), nullptr, pixelShader.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreatePixelShader failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> elements(args.elements.size());
		for (size_t ielement = 0; ielement < args.elements.size(); ++ielement)
		{
			elements[ielement].SemanticName = args.elements[ielement].name.c_str();
			elements[ielement].Format = FromFormat(args.elements[ielement].format);
			elements[ielement].AlignedByteOffset = args.elements[ielement].offset;
			elements[ielement].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		}

		core::comptr<ID3D11InputLayout> inputLayout;
		hr = device->CreateInputLayout(elements.data(), (uint32_t)elements.size(), vertexShaderCode->GetBufferPointer(), vertexShaderCode->GetBufferSize(), inputLayout.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateInputLayout failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
			return core::e_inner;
		}


		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = args.blend.alphaToCoverage;
		blendDesc.IndependentBlendEnable = args.blend.independentBlend;
		for (uint32_t itarget = 0; itarget < args.ntargets; ++itarget)
		{
			blendDesc.RenderTarget[itarget].BlendEnable = args.blend.targets[itarget].enable;
			blendDesc.RenderTarget[itarget].SrcBlend = FromBlend(args.blend.targets[itarget].srcColor);
			blendDesc.RenderTarget[itarget].DestBlend = FromBlend(args.blend.targets[itarget].destColor);
			blendDesc.RenderTarget[itarget].BlendOp = FromBlendOP(args.blend.targets[itarget].colorOP);
			blendDesc.RenderTarget[itarget].SrcBlendAlpha = FromBlend(args.blend.targets[itarget].srcAlpha);
			blendDesc.RenderTarget[itarget].DestBlendAlpha = FromBlend(args.blend.targets[itarget].destAlpha);
			blendDesc.RenderTarget[itarget].BlendOpAlpha = FromBlendOP(args.blend.targets[itarget].alphaOP);
			blendDesc.RenderTarget[itarget].RenderTargetWriteMask = FromWriteMasks(args.blend.targets[itarget].writeMasks);
		}
		core::comptr<ID3D11BlendState> blendState;
		hr = device->CreateBlendState(&blendDesc, blendState.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateBlendState failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
			return core::e_inner;
		}

		D3D11_RASTERIZER_DESC rasDesc = {};
		rasDesc.FillMode = args.rasterize.wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		rasDesc.CullMode = FromCullMode(args.rasterize.cullmode);
		rasDesc.FrontCounterClockwise = args.rasterize.CCW;
		rasDesc.DepthBias = args.rasterize.depthBias;
		rasDesc.DepthBiasClamp = 0;
		rasDesc.DepthClipEnable = args.rasterize.depthClip;
		rasDesc.MultisampleEnable = args.rasterize.MSAA;
		core::comptr<ID3D11RasterizerState> rasterizerState;
		hr = device->CreateRasterizerState(&rasDesc, rasterizerState.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateRasterizerState failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
			return core::e_inner;
		}

		D3D11_DEPTH_STENCIL_DESC depthstencilDesc = {};
		depthstencilDesc.DepthEnable = args.depthstencil.depth;
		depthstencilDesc.DepthWriteMask = args.depthstencil.depthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		depthstencilDesc.DepthFunc = FromComparison(args.depthstencil.depthComparison);
		depthstencilDesc.StencilEnable = args.depthstencil.stencil;
		core::comptr<ID3D11DepthStencilState> depthstencilState;
		hr = device->CreateDepthStencilState(&depthstencilDesc, depthstencilState.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDepthStencilState failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
			return core::e_inner;
		}
		

		_args = args;
		_inputLayout = inputLayout;
		_vertexShader = vertexShader;
		_hullShader = hullShader;
		_domainShader = domainShader;
		_geometryShader = geometryShader;
		_pixelShader = pixelShader;

		_blendState = blendState;
		_depthstencilState = depthstencilState;
		_rasterizerState = rasterizerState; 
		return core::ok;
	}

	core::error RHID3D11PipelineState::_CreateComputePipelineState(const PipelineStateArgs & args)
	{
		HRESULT hr = S_OK;
		auto device = _device->Device();
		assert(device);
		
		UINT compileFlags = 0;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		core::comptr<ID3DBlob> errorMessage;

		core::comptr<ID3DBlob> computerShaderCode;
		hr = D3DCompileFromFile(core::u8str_wstr(args.CS).c_str(), nullptr, nullptr, args.CSMain.c_str(), "cs_5_0", compileFlags, 0, computerShaderCode.getpp(), errorMessage.getpp_safe());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " D3DCompileFromFile<VS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D11BlobMessage(errorMessage);
			return core::e_inner;
		}

		core::comptr<ID3D11ComputeShader> computeShader;
		hr = device->CreateComputeShader(computerShaderCode->GetBufferPointer(), computerShaderCode->GetBufferSize(), nullptr, computeShader.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateComputeShader failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_args = args;
		_computeShader = computeShader;
		return core::ok;
	}
}
