#pragma once

#include "common.h"

class SnObject {
public:
    SnObject();
    uint64 GetSN() const;
    void SetSN(uint64 sn);

protected:
    uint64 _sn;
};