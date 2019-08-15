#pragma once

#include "RHI/RHI.h"
#include "RHI/RHICommandQueue.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	class RHID3D11PipelineState : public RHIPipelineState
	{
	public:
		RHID3D11PipelineState(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11PipelineState() = default;

		core::error Create(const PipelineStateArgs & args);
		void SetName(const std::u8string & name);

	private:
		core::error _CreateGraphicsPipelineState(const PipelineStateArgs & args);
		core::error _CreateComputePipelineState(const PipelineStateArgs & args);
		
	public:
		const PipelineStateArgs & Args() const { return _args; }
		const PipelineStateTable & Table(size_t index) const { return _args.tables[index]; }

		ID3D11InputLayout * InputLayout() const { return _inputLayout.get(); }
		ID3D11VertexShader * VertexShader() const { return _vertexShader.get(); }
		ID3D11HullShader * HullShader() const { return _hullShader.get(); }
		ID3D11DomainShader * DomainShader() const { return _domainShader.get(); }
		ID3D11GeometryShader * GeometryShader() const { return _geometryShader.get(); }
		ID3D11PixelShader * PixelShader() const { return _pixelShader.get(); }
		ID3D11ComputeShader * ComputeShader() const { return _computeShader.get(); }
		ID3D11BlendState * BlendState() const { return _blendState.get(); }
		ID3D11DepthStencilState * DepthStencilState() const { return _depthstencilState.get(); }
		ID3D11RasterizerState * RasterizerState() const { return _rasterizerState.get(); }

	private:
		RHID3D11Device * _device = nullptr;

		PipelineStateArgs _args;

		core::comptr<ID3D11InputLayout> _inputLayout;
		
		core::comptr<ID3D11VertexShader> _vertexShader;
		core::comptr<ID3D11HullShader> _hullShader;
		core::comptr<ID3D11DomainShader> _domainShader;
		core::comptr<ID3D11GeometryShader> _geometryShader;
		core::comptr<ID3D11PixelShader> _pixelShader;
		core::comptr<ID3D11ComputeShader> _computeShader;

		core::comptr<ID3D11BlendState> _blendState;
		core::comptr<ID3D11DepthStencilState> _depthstencilState;
		core::comptr<ID3D11RasterizerState> _rasterizerState;
		
		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;
	};
}
