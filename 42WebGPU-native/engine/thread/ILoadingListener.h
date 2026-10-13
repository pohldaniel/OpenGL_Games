#pragma once
#pragma once

class ILoadingListener {
public:
    virtual ~ILoadingListener() = default;
    virtual void OnProcess() = 0;
    virtual void OnComplete() = 0;
};