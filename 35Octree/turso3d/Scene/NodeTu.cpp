// For conditions of distribution and use, see copyright notice in License.txt

#include "../IO/Log.h"
#include "../IO/Stream.h"
#include "../Object/ObjectResolver.h"
#include "../Resource/JSONFile.h"
#include "Scene.h"

static std::vector<SharedPtr<NodeTu> > noChildren;
static Allocator<NodeImpl> nodeImplAllocator;

NodeTu::NodeTu() :
    impl(nodeImplAllocator.Allocate()),
    parent(nullptr),
    flags(NF_ENABLED),
    layer(LAYER_DEFAULT)
{
    impl->scene = nullptr;
    impl->id = 0;
}

NodeTu::~NodeTu()
{
    RemoveAllChildren();
    // At the time of destruction the node should not have a parent, or be in a scene
    assert(!parent);
    assert(!impl->scene);

    nodeImplAllocator.Free(impl);
}

void NodeTu::RegisterObject()
{
    RegisterFactory<NodeTu>();
    RegisterRefAttribute("name", &NodeTu::Name, &NodeTu::SetName);
    RegisterAttribute("enabled", &NodeTu::IsEnabled, &NodeTu::SetEnabled, true);
    RegisterAttribute("temporary", &NodeTu::IsTemporary, &NodeTu::SetTemporary, false);
    RegisterAttribute("layer", &NodeTu::Layer, &NodeTu::SetLayer, LAYER_DEFAULT);
}

void NodeTu::Load(Stream& source, ObjectResolver& resolver)
{
    // Load child nodes before own attributes to enable e.g. AnimatedModel to set bones at load time
    size_t numChildren = source.ReadVLE();
    children.reserve(numChildren);

    for (size_t i = 0; i < numChildren; ++i)
    {
        StringHash childType(source.Read<StringHash>());
        unsigned childId = source.Read<unsigned>();
        NodeTu* child = CreateChild(childType);
        if (child)
        {
            resolver.StoreObject(childId, child);
            child->Load(source, resolver);
        }
        else
        {
            // If child is unknown type, skip all its attributes and children
            SkipHierarchy(source);
        }
    }

    // Type and id has been read by the parent
    Serializable::Load(source, resolver);
}

void NodeTu::Save(Stream& dest)
{
    // Write type and ID first, followed by child nodes and attributes
    dest.Write(Type());
    dest.Write(Id());
    dest.WriteVLE(NumPersistentChildren());

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        NodeTu* child = *it;
        if (!child->IsTemporary())
            child->Save(dest);
    }

    Serializable::Save(dest);
}

void NodeTu::LoadJSON(const JSONValue& source, ObjectResolver& resolver)
{
    const JSONArray& childArray = source["children"].GetArray();
    children.reserve(childArray.size());

    for (auto it = childArray.begin(); it != childArray.end(); ++it)
    {
        const JSONValue& childJSON = *it;
        StringHash childType(childJSON["type"].GetString());
        unsigned childId = (unsigned)childJSON["id"].GetNumber();
        NodeTu* child = CreateChild(childType);
        if (child)
        {
            resolver.StoreObject(childId, child);
            child->LoadJSON(childJSON, resolver);
        }
    }

    // Type and id has been read by the parent
    Serializable::LoadJSON(source, resolver);
}

void NodeTu::SaveJSON(JSONValue& dest)
{
    dest["type"] = TypeName();
    dest["id"] = Id();

    if (NumPersistentChildren())
    {
        JSONValue& childArray = dest["children"];
        for (auto it = children.begin(); it != children.end(); ++it)
        {
            NodeTu* child = *it;
            if (!child->IsTemporary())
            {
                JSONValue childJSON;
                child->SaveJSON(childJSON);
                childArray.Push(childJSON);
            }
        }
    }

    Serializable::SaveJSON(dest);
}

bool NodeTu::SaveJSON(Stream& dest)
{
    JSONFile json;
    SaveJSON(json.Root());
    return json.Save(dest);
}

void NodeTu::SetName(const std::string& newName)
{
    impl->name = newName;
    impl->nameHash = StringHash(newName);
}

void NodeTu::SetName(const char* newName)
{
    impl->name = newName;
    impl->nameHash = StringHash(newName);
}

void NodeTu::SetLayer(unsigned char newLayer)
{
    if (layer < 32)
    {
        layer = newLayer;
        OnLayerChanged(newLayer);
    }
    else
        LOGERROR("Can not set layer 32 or higher");
}

void NodeTu::SetEnabled(bool enable)
{
    if (enable != TestFlag(NF_ENABLED))
    {
        SetFlag(NF_ENABLED, enable);
        OnEnabledChanged(enable);
    }
}

void NodeTu::SetEnabledRecursive(bool enable)
{
    SetEnabled(enable);
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        child->SetEnabledRecursive(enable);
    }
}

void NodeTu::SetTemporary(bool enable)
{
    SetFlag(NF_TEMPORARY, enable);
}

void NodeTu::SetParent(NodeTu* newParent)
{
    if (newParent)
        newParent->AddChild(this);
    else if (parent)
        parent->RemoveChild(this);
}

NodeTu* NodeTu::CreateChild(StringHash childType)
{
    ObjectTu* newObject = Create(childType);
    if (!newObject)
    {
        LOGERROR("Could not create child node of unknown type " + childType.ToString());
        return nullptr;
    }

#ifdef _DEBUG
    Node* child = dynamic_cast<Node*>(newObject);
    if (!child)
    {
        LOGERROR(newObject->TypeName() + " is not a Node subclass, could not add as a child");
        Destroy(newObject);
        return nullptr;
    }
#else
	NodeTu* child = static_cast<NodeTu*>(newObject);
#endif

    AddChild(child);
    return child;
}

NodeTu* NodeTu::CreateChild(StringHash childType, const std::string& childName)
{
    return CreateChild(childType, childName.c_str());
}

NodeTu* NodeTu::CreateChild(StringHash childType, const char* childName)
{
	NodeTu* child = CreateChild(childType);
    if (child)
        child->SetName(childName);
    return child;
}

void NodeTu::AddChild(NodeTu* child)
{
    // Check for illegal or redundant parent assignment
    if (!child || child->parent == this)
        return;
    
#ifdef _DEBUG
    // Check for possible illegal or cyclic parent assignment
    if (child == this)
    {
        LOGERROR("Attempted parenting node to self");
        return;
    }

    Node* current = parent;
    while (current)
    {
        if (current == child)
        {
            LOGERROR("Attempted cyclic node parenting");
            return;
        }
        current = current->parent;
    }
#endif

	NodeTu* oldParent = child->parent;
    if (oldParent)
    {
        for (auto it = oldParent->children.begin(); it != oldParent->children.end(); ++it)
        {
            if (*it == child)
            {
                oldParent->children.erase(it);
                break;
            }
        }
    }

    children.push_back(child);
    child->parent = this;
    child->OnParentSet(this, oldParent);
    if (impl->scene)
        impl->scene->AddNode(child);
}

void NodeTu::RemoveChild(NodeTu* child)
{
    if (!child || child->parent != this)
        return;

    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i] == child)
        {
            RemoveChild(i);
            break;
        }
    }
}

void NodeTu::RemoveChild(size_t index)
{
    if (index >= children.size())
        return;

	NodeTu* child = children[index];
    // Detach from both the parent and the scene (removes id assignment)
    child->parent = nullptr;
    child->SetFlag(NF_SPATIAL_PARENT, false);
    if (impl->scene)
        impl->scene->RemoveNode(child);
    children.erase(children.begin() + index);
}

void NodeTu::RemoveAllChildren()
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        child->parent = nullptr;
        child->SetFlag(NF_SPATIAL_PARENT, false);
        if (impl->scene)
            impl->scene->RemoveNode(child);
        it->Reset();
    }

    children.clear();
}

void NodeTu::RemoveSelf()
{
    if (parent)
        parent->RemoveChild(this);
}

size_t NodeTu::NumPersistentChildren() const
{
    size_t ret = 0;

    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (!child->IsTemporary())
            ++ret;
    }

    return ret;
}

void NodeTu::FindAllChildren(std::vector<NodeTu*>& result) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        result.push_back(child);
        child->FindAllChildren(result);
    }
}

NodeTu* NodeTu::FindChild(const std::string& childName, bool recursive) const
{
    return FindChild(childName.c_str(), recursive);
}

NodeTu* NodeTu::FindChild(const char* childName, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (child->impl->name == childName)
            return child;
        else if (recursive && child->children.size())
        {
			NodeTu* childResult = child->FindChild(childName, recursive);
            if (childResult)
                return childResult;
        }
    }

    return nullptr;
}

NodeTu* NodeTu::FindChild(StringHash childNameHash, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (child->impl->nameHash == childNameHash)
            return child;
        else if (recursive && child->children.size())
        {
			NodeTu* childResult = child->FindChild(childNameHash, recursive);
            if (childResult)
                return childResult;
        }
    }

    return nullptr;
}

NodeTu* NodeTu::FindChildOfType(StringHash childType, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (child->Type() == childType || DerivedFrom(child->Type(), childType))
            return child;
        else if (recursive && child->children.size())
        {
			NodeTu* childResult = child->FindChild(childType, recursive);
            if (childResult)
                return childResult;
        }
    }

    return nullptr;
}

NodeTu* NodeTu::FindChildOfType(StringHash childType, const std::string& childName, bool recursive) const
{
    return FindChildOfType(childType, childName.c_str(), recursive);
}

NodeTu* NodeTu::FindChildOfType(StringHash childType, const char* childName, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if ((child->Type() == childType || DerivedFrom(child->Type(), childType)) && child->impl->name == childName)
            return child;
        else if (recursive && child->children.size())
        {
			NodeTu* childResult = child->FindChildOfType(childType, childName, recursive);
            if (childResult)
                return childResult;
        }
    }

    return nullptr;
}

NodeTu* NodeTu::FindChildOfType(StringHash childType, StringHash childNameHash, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if ((child->Type() == childType || DerivedFrom(child->Type(), childType)) && child->impl->nameHash == childNameHash)
            return child;
        else if (recursive && child->children.size())
        {
			NodeTu* childResult = child->FindChildOfType(childType, childNameHash, recursive);
            if (childResult)
                return childResult;
        }
    }

    return nullptr;
}
NodeTu* NodeTu::FindChildByLayer(unsigned layerMask, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (child->LayerMask() && layerMask)
            return child;
        else if (recursive && child->children.size())
        {
			NodeTu* childResult = child->FindChildByLayer(layerMask, recursive);
            if (childResult)
                return childResult;
        }
    }

    return nullptr;
}

void NodeTu::FindChildren(std::vector<NodeTu*>& result, StringHash childType, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (child->Type() == childType || DerivedFrom(child->Type(), childType))
            result.push_back(child);
        if (recursive && child->children.size())
            child->FindChildren(result, childType, recursive);
    }
}

void NodeTu::FindChildrenByLayer(std::vector<NodeTu*>& result, unsigned layerMask, bool recursive) const
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
		NodeTu* child = *it;
        if (child->LayerMask() & layerMask)
            result.push_back(child);
        if (recursive && child->children.size())
            child->FindChildrenByLayer(result, layerMask, recursive);
    }
}

void NodeTu::SetScene(Scene* newScene)
{
    Scene* oldScene = impl->scene;
    impl->scene = newScene;
    OnSceneSet(impl->scene, oldScene);
}

void NodeTu::SetId(unsigned newId)
{
    impl->id = newId;
}

void NodeTu::SkipHierarchy(Stream& source)
{
    Serializable::Skip(source);

    size_t numChildren = source.ReadVLE();
    for (size_t i = 0; i < numChildren; ++i)
    {
        source.Read<StringHash>(); // StringHash childType
        source.Read<unsigned>(); // unsigned childId
        SkipHierarchy(source);
    }
}

void NodeTu::OnParentSet(NodeTu*, NodeTu*)
{
}

void NodeTu::OnSceneSet(Scene*, Scene*)
{
}

void NodeTu::OnEnabledChanged(bool)
{
}

void NodeTu::OnLayerChanged(unsigned char)
{
}
