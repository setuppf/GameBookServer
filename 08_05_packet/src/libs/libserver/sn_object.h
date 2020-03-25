#pragma once

#include "common.h"

class SnObject {
public:
    SnObject();
    SnObject(uint64 sn);
    uint64 GetSN() const;
    void ResetSN();

protected:
    uint64 _sn;
};