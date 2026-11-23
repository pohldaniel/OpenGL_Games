#include <vector>
#include <webgpu.h>

class WgpBindGroupLayoutBuilder {

public:
    WgpBindGroupLayoutBuilder(WGPUDevice device) : m_device(device) {}

    WgpBindGroupLayoutBuilder& addUniformBuffer(uint32_t binding, WGPUShaderStage visibility, uint64_t minSize) {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.buffer.type = WGPUBufferBindingType_Uniform;
        entry.buffer.minBindingSize = minSize;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupLayoutBuilder& addStorageBuffer(uint32_t binding, WGPUShaderStage visibility, uint64_t minSize, bool readOnly = false) {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.buffer.type = readOnly ? WGPUBufferBindingType_ReadOnlyStorage : WGPUBufferBindingType_Storage;
        entry.buffer.minBindingSize = minSize;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupLayoutBuilder& addTexture2D(uint32_t binding, WGPUShaderStage visibility, WGPUTextureSampleType sampleType = WGPUTextureSampleType_Float) {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.texture.sampleType = sampleType;
        entry.texture.viewDimension = WGPUTextureViewDimension_2D;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupLayoutBuilder& addTexture3D(uint32_t binding, WGPUShaderStage visibility, WGPUTextureSampleType sampleType = WGPUTextureSampleType_Float) {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.texture.sampleType = sampleType;
        entry.texture.viewDimension = WGPUTextureViewDimension_3D;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupLayoutBuilder& addStorageTexture3D(uint32_t binding, WGPUShaderStage visibility, WGPUTextureFormat format, WGPUStorageTextureAccess access = WGPUStorageTextureAccess_WriteOnly) {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.storageTexture.access = access;
        entry.storageTexture.format = format;
        entry.storageTexture.viewDimension = WGPUTextureViewDimension_3D;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupLayoutBuilder& addSampler(uint32_t binding, WGPUShaderStage visibility, WGPUSamplerBindingType type = WGPUSamplerBindingType_Filtering) {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.sampler.type = type;
        m_entries.push_back(entry);
        return *this;
    }

    WGPUBindGroupLayout build() const {
        WGPUBindGroupLayoutDescriptor desc = {};
        desc.entryCount = static_cast<uint32_t>(m_entries.size());
        desc.entries = m_entries.data();
        return wgpuDeviceCreateBindGroupLayout(m_device, &desc);
    }

private:
    WGPUDevice m_device;
    std::vector<WGPUBindGroupLayoutEntry> m_entries;
};