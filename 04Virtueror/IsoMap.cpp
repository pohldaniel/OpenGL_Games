#include "IsoMap.h"

IsoMap::IsoMap() {}

IsoMap::~IsoMap() {}

void IsoMap::render() {

}

IsoLayer * IsoMap::CreateLayer(unsigned int layerId) {
	// check layer has not been created yet
	IsoLayer * layer = GetLayer(layerId);

	if (layer != nullptr)
		return layer;

	// create and store new layer
	layer = new IsoLayer();

	mLayers.emplace_back(layer);
	mLayersMap.insert({ layerId, layer });
	mLayersRenderList.emplace_back(layer);

	return layer;
}
