#pragma once

#include "common.h"

class SnObject {
public:
    SnObject();
    uint64 GetSN() const;
    void ResetSN(bool isClean = false);

protected:
    uint64 _sn;
};