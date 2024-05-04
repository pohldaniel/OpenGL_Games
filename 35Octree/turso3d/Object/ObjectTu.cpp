// For conditions of distribution and use, see copyright notice in License.txt

#include "ObjectTu.h"
#include "../IO/JSONValue.h"

std::map<StringHash, ObjectTu*> ObjectTu::subsystems;
std::map<StringHash, AutoPtr<ObjectFactory> > ObjectTu::factories;
std::set<std::pair<StringHash, StringHash> > ObjectTu::derivedTypes;
std::map<StringHash, StringHash> ObjectTu::baseTypes;

ObjectFactory::~ObjectFactory()
{
}

void ObjectTu::ReleaseRef()
{
    assert(refCount && refCount->refs > 0);
    --(refCount->refs);
    if (refCount->refs == 0)
        Destroy(this);
}

void ObjectTu::SubscribeToEvent(EventTu& event, EventHandler* handler)
{
    event.Subscribe(handler);
}

void ObjectTu::UnsubscribeFromEvent(EventTu& event)
{
    event.Unsubscribe(this);
}

void ObjectTu::SendEvent(EventTu& event)
{
    event.Send(this);
}

bool ObjectTu::SubscribedToEvent(const EventTu& event) const
{
    return event.HasReceiver(this);
}

void ObjectTu::RegisterSubsystem(ObjectTu* subsystem)
{
    if (!subsystem)
        return;
    
    subsystems[subsystem->Type()] = subsystem;
}

void ObjectTu::RemoveSubsystem(ObjectTu* subsystem)
{
    if (!subsystem)
        return;
    
    auto it = subsystems.find(subsystem->Type());
    if (it != subsystems.end() && it->second == subsystem)
        subsystems.erase(it);
}

void ObjectTu::RemoveSubsystem(StringHash type)
{
    subsystems.erase(type);
}

ObjectTu* ObjectTu::Subsystem(StringHash type)
{
    auto it = subsystems.find(type);
    return it != subsystems.end() ? it->second : nullptr;
}

void ObjectTu::RegisterFactory(ObjectFactory* factory)
{
    if (!factory)
        return;
    
    factories[factory->Type()] = factory;
}

ObjectTu* ObjectTu::Create(StringHash type)
{
    auto it = factories.find(type);
    return it != factories.end() ? it->second->Create() : nullptr;
}

void ObjectTu::Destroy(ObjectTu* object)
{
    assert(object);

    auto it = factories.find(object->Type());
    if (it != factories.end())
        it->second->Destroy(object);
    else
        delete object;
}

const std::string& ObjectTu::TypeNameFromType(StringHash type)
{
    auto it = factories.find(type);
    return it != factories.end() ? it->second->TypeName() : JSONValue::emptyString;
}

bool ObjectTu::DerivedFrom(StringHash derived, StringHash base)
{
    return derivedTypes.find(std::make_pair(derived, base)) != derivedTypes.end();
}

void ObjectTu::RegisterDerivedType(StringHash derived, StringHash base)
{
    baseTypes[derived] = base;

    derivedTypes.insert(std::make_pair(derived, base));
    
    auto it = baseTypes.find(base);
    // Register the whole chain
    while (it != baseTypes.end())
    {
        base = it->second;
        derivedTypes.insert(std::make_pair(derived, base));
        it = baseTypes.find(base);
    }
}
