#include "stdafx.h"
#include "RHID3D12PipelineState.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12PipelineState::Create(const PipelineStateArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		assert(device);

		std::vector<D3D12_ROOT_PARAMETER1> d3d12params(args.tables.size());
		size_t nranges = 0;
		for (size_t itable = 0; itable < args.tables.size(); ++itable)
			nranges += args.tables[itable].ranges.size();

		std::vector<D3D12_DESCRIPTOR_RANGE1> ranges(nranges);
		for(size_t itable = 0, irangebase = 0; itable < args.tables.size(); ++itable)
		{
			const PipelineStateTable & table = args.tables[itable];
			d3d12params[itable].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			d3d12params[itable].ShaderVisibility = FromShader(table.shader);
			d3d12params[itable].DescriptorTable.NumDescriptorRanges = table.ranges.size();
			d3d12params[itable].DescriptorTable.pDescriptorRanges = &(ranges[irangebase]);
			for (size_t irange = 0; irange < table.ranges.size(); ++irange)
			{
				const PipelineStateTableRange & range = table.ranges[irange];
				ranges[irangebase + irange].RangeType = FromDescripteorRangeType(range.type);
				ranges[irangebase + irange].NumDescriptors = range.numDescriptor;
				ranges[irangebase + irange].BaseShaderRegister = range.shaderRegister;
				ranges[irangebase + irange].RegisterSpace = range.registerSpace;
				ranges[irangebase + irange].Flags = FromDescriptorFlags(range.flags);
				ranges[irangebase + irange].OffsetInDescriptorsFromTableStart = range.packetOffset == core::uint32_max ?  D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND : range.packetOffset;

				//if (range.type == DescriptorRangeType::ConstBuffer)
				//	ranges[irangebase + irange].Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			}
			irangebase += table.ranges.size();
		}

		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers(args.samplers.size());
		for (size_t isampler = 0; isampler < args.samplers.size(); ++isampler)
		{
			samplers[isampler].Filter = FromFilter(args.samplers[isampler].filter);
			samplers[isampler].AddressU = FromAddressMode(args.samplers[isampler].addressU);
			samplers[isampler].AddressV = FromAddressMode(args.samplers[isampler].addressV);
			samplers[isampler].AddressW = FromAddressMode(args.samplers[isampler].addressW);
			samplers[isampler].MipLODBias = args.samplers[isampler].mipLODBias;
			samplers[isampler].MaxAnisotropy = args.samplers[isampler].anisotropy;
			samplers[isampler].ComparisonFunc = FromComparison(args.samplers[isampler].comparison);
			samplers[isampler].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			samplers[isampler].MinLOD = args.samplers[isampler].minLOD;
			samplers[isampler].MaxLOD = args.samplers[isampler].maxLOD;
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSign = {};
		rootSign.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSign.Desc_1_1.NumParameters = d3d12params.size();
		rootSign.Desc_1_1.pParameters = d3d12params.data();
		rootSign.Desc_1_1.NumStaticSamplers = samplers.size();
		rootSign.Desc_1_1.pStaticSamplers = samplers.data();
		rootSign.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		core::comptr<ID3DBlob> signatureBlob;
		core::comptr<ID3DBlob> signatureBlog_error;
		hr = D3D12SerializeVersionedRootSignature(&rootSign, signatureBlob.getpp(), signatureBlog_error.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " D3D12SerializeVersionedRootSignature failed: " << win32::winerr_str(hr & 0xFFFF) << "-> " << static_cast<const char *>(signatureBlog_error->GetBufferPointer());
			return core::e_inner;
		}

		core::comptr<ID3D12RootSignature> rootSignature;
		hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), rootSignature.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateRootSignature failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		if (args.CS.empty())
			return _CreateGraphicsPipelineState(args, rootSignature);
		else
			return _CreateComputePipelineState(args, rootSignature);
	}

	void RHID3D12PipelineState::SetName(const std::u8string & name)
	{
		SetD3D12ObjectName(_pipelineState.get(), name + u8"._pipelineState");
		SetD3D12ObjectName(_rootSignature.get(), name + u8"._rootSignature");
	}
	
	core::error RHID3D12PipelineState::_CreateGraphicsPipelineState(const PipelineStateArgs & args, core::comptr<ID3D12RootSignature> rootSignature)
	{
		HRESULT hr = S_OK;

		auto device = _device->Device();
		assert(device);
		
		UINT compileFlags = 0;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		core::comptr<ID3DBlob> errorMessage;

		core::comptr<ID3DBlob> vertexShader;
		if (!args.VS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.VS).c_str(), nullptr, nullptr, args.VSMain.c_str(), "vs_5_0", compileFlags, 0, vertexShader.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<VS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D12BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> hullShader;
		if (!args.HS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.VS).c_str(), nullptr, nullptr, args.HSMain.c_str(), "hs_5_0", compileFlags, 0, hullShader.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<HS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D12BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> domainShader;
		if (!args.DS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.VS).c_str(), nullptr, nullptr, args.DSMain.c_str(), "ds_5_0", compileFlags, 0, domainShader.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<DS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D12BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> geometryShader;
		if (!args.GS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.GS).c_str(), nullptr, nullptr, args.GSMain.c_str(), "gs_5_0", compileFlags, 0, geometryShader.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<GS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D12BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		core::comptr<ID3DBlob> pixelShader;
		if (!args.PS.empty())
		{
			hr = D3DCompileFromFile(core::u8str_wstr(args.PS).c_str(), nullptr, nullptr, args.PSMain.c_str(), "ps_5_0", compileFlags, 0, pixelShader.getpp(), errorMessage.getpp_safe());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " D3DCompileFromFile<PS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D12BlobMessage(errorMessage);
				return core::e_inner;
			}
		}

		std::vector<D3D12_INPUT_ELEMENT_DESC> elements(args.elements.size());
		for (size_t ielement = 0; ielement < args.elements.size(); ++ielement)
		{
			elements[ielement].SemanticName = args.elements[ielement].name.c_str();
			elements[ielement].Format = FromFormat(args.elements[ielement].format);
			elements[ielement].AlignedByteOffset = args.elements[ielement].offset;
			elements[ielement].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = rootSignature.get();
		desc.VS = vertexShader ? D3D12_SHADER_BYTECODE{ vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() } : D3D12_SHADER_BYTECODE{};
		desc.HS = hullShader ? D3D12_SHADER_BYTECODE{ hullShader->GetBufferPointer(), hullShader->GetBufferSize() } : D3D12_SHADER_BYTECODE{};
		desc.DS = domainShader ? D3D12_SHADER_BYTECODE{ domainShader->GetBufferPointer(), domainShader->GetBufferSize() } : D3D12_SHADER_BYTECODE{};
		desc.GS = geometryShader ? D3D12_SHADER_BYTECODE{ geometryShader->GetBufferPointer(), geometryShader->GetBufferSize() } : D3D12_SHADER_BYTECODE{};
		desc.PS = vertexShader ? D3D12_SHADER_BYTECODE{ pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() } : D3D12_SHADER_BYTECODE{};

		desc.BlendState.AlphaToCoverageEnable = args.blend.alphaToCoverage;
		desc.BlendState.IndependentBlendEnable = args.blend.independentBlend;
		for (uint32_t itarget = 0; itarget < args.ntargets; ++itarget)
		{
			desc.BlendState.RenderTarget[itarget].BlendEnable = args.blend.targets[itarget].enable;
			desc.BlendState.RenderTarget[itarget].SrcBlend = FromBlend(args.blend.targets[itarget].srcColor);
			desc.BlendState.RenderTarget[itarget].DestBlend = FromBlend(args.blend.targets[itarget].destColor);
			desc.BlendState.RenderTarget[itarget].BlendOp = FromBlendOP(args.blend.targets[itarget].colorOP);
			desc.BlendState.RenderTarget[itarget].SrcBlendAlpha = FromBlend(args.blend.targets[itarget].srcAlpha);
			desc.BlendState.RenderTarget[itarget].DestBlendAlpha = FromBlend(args.blend.targets[itarget].destAlpha);
			desc.BlendState.RenderTarget[itarget].BlendOpAlpha = FromBlendOP(args.blend.targets[itarget].alphaOP);
			desc.BlendState.RenderTarget[itarget].LogicOp = D3D12_LOGIC_OP_NOOP;
			desc.BlendState.RenderTarget[itarget].RenderTargetWriteMask = FromWriteMasks(args.blend.targets[itarget].writeMasks);
		}

		desc.DepthStencilState.DepthEnable = args.depthstencil.depth;
		desc.DepthStencilState.StencilEnable = args.depthstencil.stencil;

		desc.InputLayout = { elements.data(), (uint32_t)elements.size() };
		desc.PrimitiveTopologyType = FromTopologyType(args.topology);

		desc.NumRenderTargets = args.ntargets;
		for (uint32_t itarget = 0; itarget < args.ntargets; ++itarget)
			desc.RTVFormats[itarget] = FromFormat(args.formats[itarget]);

		desc.SampleMask = args.samplemask;
		desc.SampleDesc.Count = args.SampleCount;
		desc.SampleDesc.Quality = args.SampleQuality;

		desc.RasterizerState.FillMode = args.rasterize.wireframe ? D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.CullMode = FromCullMode(args.rasterize.cullmode);
		desc.RasterizerState.FrontCounterClockwise = args.rasterize.CCW;
		desc.RasterizerState.DepthBias = args.rasterize.depthBias;
		desc.RasterizerState.DepthBiasClamp = 0;
		desc.RasterizerState.DepthClipEnable = args.rasterize.depthClip;
		desc.RasterizerState.MultisampleEnable = args.rasterize.MSAA;

		core::comptr<ID3D12PipelineState> pipelineState;
		hr = device->CreateGraphicsPipelineState(&desc, __uuidof(ID3D12PipelineState), pipelineState.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateGraphicsPipelineState failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_args = args;
		_rootSignature = rootSignature;
		_pipelineState = pipelineState;
		return core::ok;
	}

	core::error RHID3D12PipelineState::_CreateComputePipelineState(const PipelineStateArgs & args, core::comptr<ID3D12RootSignature> rootSignature)
	{
		HRESULT hr = S_OK;
		auto device = _device->Device();
		assert(device);
		
		UINT compileFlags = 0;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		core::comptr<ID3DBlob> errorMessage;

		core::comptr<ID3DBlob> computerShader;
		hr = D3DCompileFromFile(core::u8str_wstr(args.CS).c_str(), nullptr, nullptr, args.CSMain.c_str(), "cs_5_0", compileFlags, 0, computerShader.getpp(), errorMessage.getpp_safe());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " D3DCompileFromFile<VS> failed: " << win32::winerr_str(hr & 0xFFFF) << "-> \n" << D3D12BlobMessage(errorMessage);
			return core::e_inner;
		}

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = rootSignature.get();
		desc.CS = computerShader ? D3D12_SHADER_BYTECODE{ computerShader->GetBufferPointer(), computerShader->GetBufferSize() } : D3D12_SHADER_BYTECODE{};

		core::comptr<ID3D12PipelineState> pipelineState;
		hr = device->CreateComputePipelineState(&desc, __uuidof(ID3D12PipelineState), pipelineState.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateGraphicsPipelineState failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_args = args;
		_rootSignature = rootSignature;
		_pipelineState = pipelineState;
		return core::ok;
	}
}
