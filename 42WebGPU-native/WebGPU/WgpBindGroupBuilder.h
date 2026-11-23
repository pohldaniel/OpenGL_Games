#include <vector>
#include <webgpu.h>

class WgpBindGroupBuilder {
public:
    WgpBindGroupBuilder(WGPUDevice device, WGPUBindGroupLayout layout)
        : m_device(device), m_layout(layout) {
    }

    WgpBindGroupBuilder& bindBuffer(uint32_t binding, WGPUBuffer buffer, uint64_t size, uint64_t offset = 0) {
        WGPUBindGroupEntry entry = {};
        entry.binding = binding;
        entry.buffer = buffer;
        entry.offset = offset;
        entry.size = size;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupBuilder& bindTextureView(uint32_t binding, WGPUTextureView view) {
        WGPUBindGroupEntry entry = {};
        entry.binding = binding;
        entry.textureView = view;
        m_entries.push_back(entry);
        return *this;
    }

    WgpBindGroupBuilder& bindSampler(uint32_t binding, WGPUSampler sampler) {
        WGPUBindGroupEntry entry = {};
        entry.binding = binding;
        entry.sampler = sampler;
        m_entries.push_back(entry);
        return *this;
    }

    WGPUBindGroup build() const {
        WGPUBindGroupDescriptor desc = {};
        desc.layout = m_layout;
        desc.entryCount = static_cast<uint32_t>(m_entries.size());
        desc.entries = m_entries.data();
        return wgpuDeviceCreateBindGroup(m_device, &desc);
    }

private:
    WGPUDevice m_device;
    WGPUBindGroupLayout m_layout;
    std::vector<WGPUBindGroupEntry> m_entries;
};