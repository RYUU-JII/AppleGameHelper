// include/Shared/StopControllable.h
#pragma once

class StopControllable {
public:
    virtual ~StopControllable() = default;
    virtual void stopRequested() = 0;
};