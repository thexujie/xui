#include "stdafx.h"
#include "RHID3D11CommandList.h"
#include "RHID3D11CommandAllocator.h"
#include "RHID3D11Resource.h"
#include "RHID3D11RenderTarget.h"
#include "RHID3D11PipelineState.h"
#include "RHID3D11ResourcePacket.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11CommandList::Create(CommandType type, RHICommandAllocator * allocator)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		uint32_t flags = 0;
		core::comptr<ID3D11DeviceContext> deferredContext;
		hr = device->CreateDeferredContext(flags, deferredContext.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDeferredContext failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_deferredContext = deferredContext;
		
		return core::ok;
	}

	void RHID3D11CommandList::SetName(const std::u8string & name)
	{
	}

	void RHID3D11CommandList::Reset(RHICommandAllocator * allocator, uint32_t index)
	{
		_commandList.reset();
		_deferredContext->ClearState();
	}

	void RHID3D11CommandList::Close()
	{
		_deferredContext->FinishCommandList(true, _commandList.getpp());
	}

	void RHID3D11CommandList::SetRenderTarget(RHIRenderTarget * rendertarget, uint32_t index)
	{
		if (!rendertarget)
		{
			UINT UAVInitialCounts = 0;
			ID3D11UnorderedAccessView * uavs[D3D11_1_UAV_SLOT_COUNT] = {};
			_deferredContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, D3D11_1_UAV_SLOT_COUNT, uavs, &UAVInitialCounts);
			return;
		}
		
		auto d3d11rendertarget = static_cast<RHID3D11RenderTarget *>(rendertarget);
		ID3D11RenderTargetView * rtvs[] = { d3d11rendertarget->RenderTargetView() };

		ID3D11UnorderedAccessView * uavs[D3D11_1_UAV_SLOT_COUNT] = {};
		size_t nuavs = 0;
		if (_resourcePacket)
		{
			auto & args = _resourcePacket->ViewArgs();
			auto & views = _resourcePacket->Views();
			for (size_t iuav = 0; iuav < std::min<size_t>(D3D11_1_UAV_SLOT_COUNT, views.size()); ++iuav)
			{
				if (args[iuav].type == ResourceType::UnorderedAccess)
				{
					uavs[iuav] = views[iuav].as<ID3D11UnorderedAccessView>().get();
					++nuavs;
				}
			}
		}

		if (nuavs > 0)
		{
			UINT UAVInitialCounts = 0;
			_deferredContext->OMSetRenderTargetsAndUnorderedAccessViews(1, rtvs, nullptr, 0, D3D11_1_UAV_SLOT_COUNT, uavs, &UAVInitialCounts);
		}
		else
		{
			_deferredContext->OMSetRenderTargets(1, rtvs, nullptr);
		}
	}

	void RHID3D11CommandList::ClearRenderTarget(RHIRenderTarget * rendertarget, uint32_t index, core::color color)
	{
		auto d3d11rendertarget = static_cast<RHID3D11RenderTarget *>(rendertarget);
		const float clearColor[] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
		_deferredContext->ClearRenderTargetView(d3d11rendertarget->RenderTargetView(), clearColor);
	}

	void RHID3D11CommandList::SetViewPort(const ViewPort & viewport)
	{
		D3D11_VIEWPORT d3d12viewport = {};
		d3d12viewport.TopLeftX = viewport.left;
		d3d12viewport.TopLeftY = viewport.top;
		d3d12viewport.Width = viewport.width;
		d3d12viewport.Height = viewport.height;
		d3d12viewport.MinDepth = viewport.nearZ;
		d3d12viewport.MaxDepth = viewport.farZ;

		_deferredContext->RSSetViewports(1, &d3d12viewport);
	}

	void RHID3D11CommandList::SetScissorRect(const core::recti & rect)
	{
		D3D11_RECT d3d12rect = {};
		d3d12rect.left = rect.x;
		d3d12rect.top = rect.y;
		d3d12rect.right = rect.right();
		d3d12rect.bottom = rect.bottom();

		_deferredContext->RSSetScissorRects(1, &d3d12rect);
	}

	void RHID3D11CommandList::TransitionBarrier(RHIResource * resource, ResourceStates states)
	{
		//static_cast<RHID3D12Resource *>(resource)->TransitionBarrier(this, states);
	}
	
	void RHID3D11CommandList::TransitionBarrier(RHIRenderTarget * rendertarget, uint32_t index, ResourceStates states)
	{
		//static_cast<RHIRenderTarget *>(rendertarget)->TransitionBarrier(this, states);
	}
	
	void RHID3D11CommandList::SetPipelineState(RHIPipelineState * pipelinestate)
	{
		_pipelineState = static_cast<RHID3D11PipelineState *>(pipelinestate);
		_deferredContext->IASetInputLayout(_pipelineState->InputLayout());
		_deferredContext->VSSetShader(_pipelineState->VertexShader(), nullptr, 0);
		_deferredContext->HSSetShader(_pipelineState->HullShader(), nullptr, 0);
		_deferredContext->DSSetShader(_pipelineState->DomainShader(), nullptr, 0);
		_deferredContext->GSSetShader(_pipelineState->GeometryShader(), nullptr, 0);
		_deferredContext->PSSetShader(_pipelineState->PixelShader(), nullptr, 0);
		_deferredContext->CSSetShader(_pipelineState->ComputeShader(), nullptr, 0);
		_deferredContext->OMSetBlendState(_pipelineState->BlendState(), _pipelineState->Args().blend.blendFactor, _pipelineState->Args().samplemask);
		_deferredContext->OMSetDepthStencilState(_pipelineState->DepthStencilState(), _pipelineState->Args().depthstencil.stencilref);
		_deferredContext->RSSetState(_pipelineState->RasterizerState());
	}
	
	void RHID3D11CommandList::SetResourcePacket(RHIResourcePacket * packet)
	{
		_resourcePacket = static_cast<RHID3D11ResourcePacket *>(packet);
	}

	void RHID3D11CommandList::SetGraphicsResources(uint32_t index, uint32_t packetoffset)
	{
		assert(_resourcePacket);
		if (!_resourcePacket)
			return;

		if (!_pipelineState)
		{
			core::war() << __FUNCTION__ " no pso.";
			return;
		}

		const std::vector<RHIResource *> & resources = _resourcePacket->Resources();
		const std::vector<ResourceViewArgs> & viewArgs = _resourcePacket->ViewArgs();
		const std::vector<core::comptr<ID3D11View>> & views = _resourcePacket->Views();
		const PipelineStateTable & table = _pipelineState->Table(index);
		if (packetoffset + table.ranges.size() > resources.size())
		{
			core::war() << __FUNCTION__ " resource index out of bounds.";
			return;
		}
		
		for (size_t irange = 0; irange < table.ranges.size(); ++irange)
		{
			auto & args = viewArgs[packetoffset + irange];
			auto & range = table.ranges[irange];
			switch (args.type)
			{
			case ResourceType::ConstBuffer:
			{
				RHID3D11Resource * d3d11resource = static_cast<RHID3D11Resource *>(resources[irange]);
				core::comptr<ID3D11Resource> resource(core::com_ref(d3d11resource->Resource()));
				ID3D11Buffer * buffers[] = { resource .as<ID3D11Buffer>().get()};
				switch(table.shader)
				{
				case Shader::Vertex:
					_deferredContext->VSSetConstantBuffers(range.shaderRegister, 1, buffers);
					break;
				case Shader::Hull:
					_deferredContext->HSSetConstantBuffers(range.shaderRegister, 1, buffers);
					break;
				case Shader::Domain:
					_deferredContext->DSSetConstantBuffers(range.shaderRegister, 1, buffers);
					break;
				case Shader::Geoetry:
					_deferredContext->GSSetConstantBuffers(range.shaderRegister, 1, buffers);
					break;
				case Shader::Pixel:
					_deferredContext->PSSetConstantBuffers(range.shaderRegister, 1, buffers);
					break;
				case Shader::All:
					_deferredContext->VSSetConstantBuffers(range.shaderRegister, 1, buffers);
					_deferredContext->HSSetConstantBuffers(range.shaderRegister, 1, buffers);
					_deferredContext->DSSetConstantBuffers(range.shaderRegister, 1, buffers);
					_deferredContext->GSSetConstantBuffers(range.shaderRegister, 1, buffers);
					_deferredContext->PSSetConstantBuffers(range.shaderRegister, 1, buffers);
					break;
				default:
					break;;
				}
				break;
			}
			case ResourceType::ShaderResource:
			{
				core::comptr<ID3D11View> view(views[irange]);
				ID3D11ShaderResourceView * shaderviews[] = { view.as<ID3D11ShaderResourceView>().get() };
				switch (table.shader)
				{
				case Shader::Vertex:
					_deferredContext->VSSetShaderResources(range.shaderRegister, 1, shaderviews);
					break;
				case Shader::Hull:
					_deferredContext->HSSetShaderResources(range.shaderRegister, 1, shaderviews);
					break;
				case Shader::Domain:
					_deferredContext->DSSetShaderResources(range.shaderRegister, 1, shaderviews);
					break;
				case Shader::Geoetry:
					_deferredContext->GSSetShaderResources(range.shaderRegister, 1, shaderviews);
					break;
				case Shader::Pixel:
					_deferredContext->PSSetShaderResources(range.shaderRegister, 1, shaderviews);
					break;
				case Shader::All:
					_deferredContext->VSSetShaderResources(range.shaderRegister, 1, shaderviews);
					_deferredContext->HSSetShaderResources(range.shaderRegister, 1, shaderviews);
					_deferredContext->DSSetShaderResources(range.shaderRegister, 1, shaderviews);
					_deferredContext->GSSetShaderResources(range.shaderRegister, 1, shaderviews);
					_deferredContext->PSSetShaderResources(range.shaderRegister, 1, shaderviews);
					break;
				default:
					break;;
				}
				break;
			}
			case ResourceType::UnorderedAccess:
			{
				core::comptr<ID3D11View> view(views[irange]);
				ID3D11UnorderedAccessView * shaderviews[] = { view.as<ID3D11UnorderedAccessView>().get() };
				UINT UAVInitialCounts = 0;
				switch (table.shader)
				{
				case Shader::Vertex:
					break;
				case Shader::Hull:
					break;
				case Shader::Domain:
					break;
				case Shader::Geoetry:
					_deferredContext->CSSetUnorderedAccessViews(range.shaderRegister, 1, shaderviews, &UAVInitialCounts);
					break;
				case Shader::Pixel:
					break;
				case Shader::All:
					_deferredContext->CSSetUnorderedAccessViews(range.shaderRegister, 1, shaderviews, &UAVInitialCounts);
					break;
				default:
					break;;
				}
				break;
			}
			default:;
			}
		}
	}
	
	void RHID3D11CommandList::IASetVertexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
		core::comptr<ID3D11Resource> buffer(core::com_ref(static_cast<RHID3D11Resource *>(resource)->Resource()));
		ID3D11Buffer * buffers[] = { buffer.as<ID3D11Buffer>().get() };
		uint32_t strides[] = { stride };
		uint32_t offsets[] = { 0 };
		_deferredContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
	}
	
	void RHID3D11CommandList::IASetIndexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
		core::comptr<ID3D11Resource> buffer(core::com_ref(static_cast<RHID3D11Resource *>(resource)->Resource()));
		_deferredContext->IASetIndexBuffer(buffer.as<ID3D11Buffer>().get(), stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	}

	void RHID3D11CommandList::IASetTopologyType(Topology topology)
	{
		_deferredContext->IASetPrimitiveTopology(FromTopology(topology));
	}

	void RHID3D11CommandList::DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase)
	{
		_deferredContext->DrawInstanced(nvertices, ninstance, ivertexbase, iinstancebase);
	}

	void RHID3D11CommandList::DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase)
	{
		_deferredContext->DrawIndexedInstanced(nindices, ninstance, iindexbase, ivertexbase, iinstancebase);
	}

	void RHID3D11CommandList::SetComputeResources(uint32_t index, uint32_t packetoffset)
	{
		assert(_resourcePacket);
		if (!_resourcePacket)
			return;

		if (!_pipelineState)
		{
			core::war() << __FUNCTION__ " no pso.";
			return;
		}

		const std::vector<RHIResource *> & resources = _resourcePacket->Resources();
		const std::vector<ResourceViewArgs> & viewArgs = _resourcePacket->ViewArgs();
		const std::vector<core::comptr<ID3D11View>> & views = _resourcePacket->Views();
		const PipelineStateTable & table = _pipelineState->Table(index);
		if (packetoffset + table.ranges.size() > resources.size())
		{
			core::war() << __FUNCTION__ " resource index out of bounds.";
			return;
		}

		for (size_t iresource = 0; iresource < resources.size(); ++iresource)
		{
			auto & args = viewArgs[iresource];
			auto & range = table.ranges[packetoffset + iresource];
			switch (args.type)
			{
			case ResourceType::ConstBuffer:
			{
				core::comptr<ID3D11Resource> resource(core::com_ref(static_cast<RHID3D11Resource *>(resources[iresource])->Resource()));
				ID3D11Buffer * buffers[] = { resource.as<ID3D11Buffer>().get() };
				_deferredContext->CSSetConstantBuffers(range.shaderRegister, 1, buffers);
				break;
			}
			case ResourceType::ShaderResource:
			{
				core::comptr<ID3D11View> view(views[iresource]);
				ID3D11ShaderResourceView * shaderviews[] = { view.as<ID3D11ShaderResourceView>().get() };
				_deferredContext->CSSetShaderResources(range.shaderRegister, 1, shaderviews);
				break;
			}
			case ResourceType::UnorderedAccess:
			{
				core::comptr<ID3D11View> view(views[iresource]);
				ID3D11UnorderedAccessView * shaderviews[] = { view.as<ID3D11UnorderedAccessView>().get() };
				UINT UAVInitialCounts = 0;
				_deferredContext->CSSetUnorderedAccessViews(range.shaderRegister, 1, shaderviews, &UAVInitialCounts);
			}
			default:;
			}
		}
	}
	
	void RHID3D11CommandList::Dispatch(core::uint3 ngroups)
	{
		_deferredContext->Dispatch(ngroups.x, ngroups.y, ngroups.z);
	}
	
	void RHID3D11CommandList::CopyResource(RHIResource * dst, RHIResource * src)
	{
		auto device = _device->Device();
		auto rhid3d11dst = static_cast<RHID3D11Resource *>(dst);
		auto rhid3d11src = static_cast<RHID3D11Resource *>(src);
		if (rhid3d11dst->Args().dimension == ResourceDimension::Texture2D)
		{
			//D3D12_TEXTURE_COPY_LOCATION destLocation;
			//destLocation.pResource = rhid3d12dst->Resource();
			//destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			//destLocation.SubresourceIndex = 0;
			//
			//D3D12_RESOURCE_DESC descDest = rhid3d12dst->Resource()->GetDesc();
			//D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
			//UINT nrows = 0;
			//UINT64 stride = 0;
			//UINT64 nbtotal = 0;
			//device->GetCopyableFootprints(&descDest, 0, 1, 0, &layout, &nrows, &stride, &nbtotal);
			//D3D12_TEXTURE_COPY_LOCATION srcLocation;
			//srcLocation.pResource = rhid3d12src->Resource();
			//srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			//srcLocation.PlacedFootprint = layout;
			_deferredContext->CopySubresourceRegion(static_cast<RHID3D11Resource *>(dst)->Resource(), 0, 0, 0, 0, static_cast<RHID3D11Resource *>(rhid3d11src)->Resource(), 0, NULL);
		}
		else
		{
			_deferredContext->CopyResource(static_cast<RHID3D11Resource *>(dst)->Resource(), static_cast<RHID3D11Resource *>(src)->Resource());
		}
	}
	
	void RHID3D11CommandList::CopyBuffer(RHIResource * dst, RHIResource * src)
	{
		//auto size = static_cast<RHID3D12Resource *>(src)->Size().cx * static_cast<RHID3D12Resource *>(src)->Size().cy;
		//_cmdlist->CopyBufferRegion(static_cast<RHID3D12Resource *>(dst)->Resource(), 0, static_cast<RHID3D12Resource *>(src)->Resource(), 0, size);
	}
	
	void RHID3D11CommandList::CopyResource(RHIResource * dst, RHIResource * src, uint32_t offset, uint32_t size)
	{
		D3D11_BOX box = {};
		box.left = offset;
		box.right = offset + size;
		box.bottom = 1;
		box.back = 1;
		_deferredContext->CopySubresourceRegion(static_cast<RHID3D11Resource *>(dst)->Resource(), 0, 0, 0, 0,
			static_cast<RHID3D11Resource *>(src)->Resource(), 0, &box);
	}
}
