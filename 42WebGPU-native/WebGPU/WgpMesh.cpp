#include "WgpContext.h"
#include "WgpMesh.h"

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer) :
	m_drawCount(indexBuffer.size()),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false){

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
}

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& joints, uint32_t stride) :
	m_drawCount(indexBuffer.size()),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false){

	if (stride == 8u) {
		std::vector<VertexAnimated> _vertexBuffer;
		for (size_t i = 0u; i < vertexBuffer.size() / stride; i++) {
			_vertexBuffer.push_back({ {vertexBuffer[i * stride], vertexBuffer[i * stride + 1u] , vertexBuffer[i * stride + 2u]},
									  {vertexBuffer[i * stride + 3u] , vertexBuffer[i * stride + 4u]},
									  {vertexBuffer[i * stride + 5u], vertexBuffer[i * stride + 6u] , vertexBuffer[i * stride + 7u]},
									  {weights[i][0], weights[i][1], weights[i][2], weights[i][3]},
									  {joints[i][0], joints[i][1], joints[i][2], joints[i][3]} });
		}
		m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(_vertexBuffer.data()), sizeof(VertexAnimated) * _vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	}
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
}

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath) :
	m_drawCount(indexBuffer.size()),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	m_texture.loadFromFile(texturePath);
}

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& joints, uint32_t stride, const std::string& texturePath) :
	m_drawCount(indexBuffer.size()),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false) {

	if (stride == 8u) {
		std::vector<VertexAnimated> _vertexBuffer;
		for (size_t i = 0u; i < vertexBuffer.size() / stride; i++) {
			_vertexBuffer.push_back({ {vertexBuffer[i * stride], vertexBuffer[i * stride + 1u] , vertexBuffer[i * stride + 2u]},
									  {vertexBuffer[i * stride + 3u] , vertexBuffer[i * stride + 4u]},
									  {vertexBuffer[i * stride + 5u], vertexBuffer[i * stride + 6u] , vertexBuffer[i * stride + 7u]},
									  {weights[i][0], weights[i][1], weights[i][2], weights[i][3]},
									  {joints[i][0], joints[i][1], joints[i][2], joints[i][3]} });
		}
		m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(_vertexBuffer.data()), sizeof(VertexAnimated) * _vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	}
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	m_texture.loadFromFile(texturePath);
}

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::pair<unsigned char*, unsigned int>& texture) :
	m_drawCount(indexBuffer.size()),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
	m_texture.loadFromMemory(texture.first, texture.second);
}

WgpMesh::WgpMesh(const WgpBuffer& vertexBuffer, const WgpBuffer& indexBuffer) :
	m_vertexBuffer(vertexBuffer),
	m_indexBuffer(indexBuffer),
	m_drawCount(wgpuBufferGetSize(indexBuffer.m_buffer) / sizeof(unsigned int)),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false) {

}

WgpMesh::WgpMesh(WgpMesh const& rhs) :
	m_vertexBuffer(rhs.m_vertexBuffer),
	m_indexBuffer(rhs.m_indexBuffer),
	m_texture(rhs.m_texture),
	m_bindGroups(rhs.m_bindGroups),
	m_drawCount(rhs.m_drawCount),
	m_bindGroupsSlot(rhs.m_bindGroupsSlot),
	m_markForDelete(false) {
}

WgpMesh::WgpMesh(WgpMesh&& rhs) noexcept :
	m_vertexBuffer(std::move(rhs.m_vertexBuffer)),
	m_indexBuffer(std::move(rhs.m_indexBuffer)),
	m_texture(std::move(rhs.m_texture)),
	m_bindGroups(std::move(rhs.m_bindGroups)),
	m_drawCount(rhs.m_drawCount),
	m_bindGroupsSlot(std::move(rhs.m_bindGroupsSlot)),
	m_markForDelete(false) {
}

WgpMesh::~WgpMesh() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpMesh::cleanup() {		
	for (auto& it : m_bindGroups) {
		for (auto& bindgroup : it.second) {
			wgpuBindGroupRelease(bindgroup);
		}
	}
}

void WgpMesh::markForDelete() {
	m_vertexBuffer.markForDelete();
	m_indexBuffer.markForDelete();
	m_texture.markForDelete();
	m_markForDelete = true;
}

void WgpMesh::setBindGroupsSlot(const std::string& bindGroupsSlot) {
	m_bindGroupsSlot = bindGroupsSlot;
}

void WgpMesh::setBindGroups(const std::string& bindGroupsName, const std::function<std::vector<WGPUBindGroup>()>& onBindGroups) {
	addBindGroups(bindGroupsName, onBindGroups);
	m_bindGroupsSlot = bindGroupsName;
}

void WgpMesh::addBindGroups(const std::string& bindGroupsName, const std::function<std::vector<WGPUBindGroup>()>& onBindGroups) {
	m_bindGroups[bindGroupsName] = onBindGroups();
}

void WgpMesh::addBindGroup(const std::string& bindGroupsName, WGPUBindGroup bindGroup) const {
	m_bindGroups[bindGroupsName].push_back(bindGroup);
}

void WgpMesh::addColor(std::array<float, 4> color) {	
	unsigned int maxIndex = 0;
	{
		WgpBuffer stagingBuffer;
		stagingBuffer.createBuffer(wgpuBufferGetSize(m_indexBuffer.getBuffer()), WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst);
		std::tuple<bool, WgpBuffer, unsigned int&> userdata = { false, stagingBuffer, maxIndex };

		WGPUBufferMapCallbackInfo bufferMapCallbackInfo = {};
		bufferMapCallbackInfo.callback = OnMapIndexBuffer;
		bufferMapCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
		bufferMapCallbackInfo.userdata1 = &userdata;

		WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, NULL);
		wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, m_indexBuffer.getBuffer(), 0u, stagingBuffer.getBuffer(), 0u, wgpuBufferGetSize(m_indexBuffer.getBuffer()));

		WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, NULL);

		wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);

		wgpuCommandBufferRelease(commandBuffer);
		wgpuCommandEncoderRelease(commandEncoder);

		wgpuBufferMapAsync(stagingBuffer.getBuffer(), WGPUMapMode_Read, 0, wgpuBufferGetSize(stagingBuffer.getBuffer()), bufferMapCallbackInfo);

		while (!std::get<0>(userdata)) {
			wgpuInstanceProcessEvents(wgpContext.instance);
		}
		stagingBuffer.markForDelete();
	}

	size_t stride = wgpuBufferGetSize(m_vertexBuffer.getBuffer()) / ((maxIndex + 1u) * sizeof(float));
	
	{
		WgpBuffer stagingBuffer;
		stagingBuffer.createBuffer(wgpuBufferGetSize(m_vertexBuffer.getBuffer()), WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst);
		std::tuple<bool, WgpBuffer, WgpBuffer&, size_t> userdata = { false, stagingBuffer , m_vertexBuffer, stride };

		WGPUBufferMapCallbackInfo bufferMapCallbackInfo = {};
		bufferMapCallbackInfo.callback = OnMapColorToBuffer;
		bufferMapCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
		bufferMapCallbackInfo.userdata1 = &userdata;
		bufferMapCallbackInfo.userdata2 = &color;

		WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, NULL);
		wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, m_vertexBuffer.getBuffer(), 0u, stagingBuffer.getBuffer(), 0u, wgpuBufferGetSize(m_vertexBuffer.getBuffer()));


		WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, NULL);

		wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);

		wgpuCommandBufferRelease(commandBuffer);
		wgpuCommandEncoderRelease(commandEncoder);

		WGPUFuture future = wgpuBufferMapAsync(stagingBuffer.getBuffer(), WGPUMapMode_Read, 0u, wgpuBufferGetSize(stagingBuffer.getBuffer()), bufferMapCallbackInfo);			
		WGPUFutureWaitInfo futureWaitInfo = {};
		futureWaitInfo.future = future;
		futureWaitInfo.completed = false;
		WGPUWaitStatus status = wgpuInstanceWaitAny(wgpContext.instance, 1u, &futureWaitInfo, UINT64_MAX);
	
		stagingBuffer.markForDelete();
	}
}

std::vector<WGPUBindGroup>& WgpMesh::getBindGroups(std::string bindGroupsName) const {
	return m_bindGroups.at(bindGroupsName);
}

std::vector<WGPUBindGroup>& WgpMesh::getBindGroups() const {
	return m_bindGroups.at(m_bindGroupsSlot);
}

const WgpTexture& WgpMesh::getTexture() const {
	return m_texture;
}

const WgpBuffer& WgpMesh::getVertexBuffer() const {
	return m_vertexBuffer;
}

const WgpBuffer& WgpMesh::getIndexBuffer() const {
	return m_indexBuffer;
}

const uint32_t WgpMesh::getDrawCount() const {
	return m_drawCount;
}

void WgpMesh::draw(const WGPURenderPassEncoder& renderPassEncoder, uint32_t instanceCount) const {
	if (m_bindGroups.size()) {
		const std::vector<WGPUBindGroup>& bindGroups = m_bindGroups.at(m_bindGroupsSlot);
		for (uint32_t i = 0u; i < bindGroups.size(); i++) {
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, bindGroups[i], 0u, NULL);
		}
	}

	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
	wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
	wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, m_drawCount, instanceCount, 0u, 0u, 0u);
}

void WgpMesh::draw(const WGPURenderBundleEncoder& renderBundleEncoder, uint32_t instanceCount) const {
	if (m_bindGroups.size()) {
		const std::vector<WGPUBindGroup>& bindGroups = m_bindGroups.at(m_bindGroupsSlot);
		for (uint32_t i = 0u; i < bindGroups.size(); i++) {
			wgpuRenderBundleEncoderSetBindGroup(renderBundleEncoder, i, bindGroups[i], 0u, NULL);
		}
	}

	wgpuRenderBundleEncoderSetVertexBuffer(renderBundleEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
	wgpuRenderBundleEncoderSetIndexBuffer(renderBundleEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
	wgpuRenderBundleEncoderDrawIndexed(renderBundleEncoder, m_drawCount, instanceCount, 0u, 0u, 0u);
}

void WgpMesh::OnMapColorToBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPUMapAsyncStatus_Success) {	
		std::tuple<bool, WgpBuffer, WgpBuffer&, size_t>* userdata = static_cast<std::tuple<bool, WgpBuffer, WgpBuffer&, size_t>*>(userdata1);
		WgpBuffer& vertexBuffer = std::get<2>(*userdata);
		size_t stride = std::get<3>(*userdata);
		uint64_t size = wgpuBufferGetSize(vertexBuffer.getBuffer());
		uint64_t floatCount = size / (sizeof(float));
		std::array<float, 4>* color = static_cast<std::array<float, 4>*>(userdata2);

		std::vector<float> vertices;
		float* bufferData = (float*)wgpuBufferGetConstMappedRange(std::get<1>(*userdata).getBuffer(), 0u, wgpuBufferGetSize(std::get<1>(*userdata).getBuffer()));
		for (uint64_t i = 0; i < floatCount; i++) {
			vertices.push_back(bufferData[i]);
			if ((i + 1) % stride == 0) {
				vertices.push_back((*color)[0]); vertices.push_back((*color)[1]); vertices.push_back((*color)[2]); vertices.push_back((*color)[3]);
			}
		}
		wgpuBufferUnmap(std::get<1>(*userdata).getBuffer());

		vertexBuffer.cleanup();
		vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertices.data()), sizeof(float) * vertices.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc);
		std::get<0>(*userdata) = true;
	}else {
		printf("Buffer message: %s", message.data);
	}
}

void WgpMesh::OnMapIndexBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPUMapAsyncStatus_Success) {
		std::tuple<bool, WgpBuffer, unsigned int&>* userdata = static_cast<std::tuple<bool, WgpBuffer, unsigned int&>*>(userdata1);

		uint64_t size = wgpuBufferGetSize(std::get<1>(*userdata).getBuffer()) / sizeof(unsigned int);

		unsigned int* bufferData = (unsigned int*)wgpuBufferGetConstMappedRange(std::get<1>(*userdata).getBuffer(), 0u, wgpuBufferGetSize(std::get<1>(*userdata).getBuffer()));

		unsigned int maxIndex = bufferData[0];
		for (uint64_t i = 1u; i < size; i++) {
			maxIndex = std::max(maxIndex, bufferData[i]);
		}
		
		wgpuBufferUnmap(std::get<1>(*userdata).getBuffer());
		std::get<0>(*userdata) = true;
		std::get<2>(*userdata) = maxIndex;
	}else {
		printf("Buffer message: %s", message.data);
	}
}