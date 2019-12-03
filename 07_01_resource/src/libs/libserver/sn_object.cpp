#include "sn_object.h"
#include "global.h"

SnObject::SnObject() {
    _sn = Global::GetInstance()->GenerateSN();
}

uint64 SnObject::GetSN() const {
    return _sn;
}

void SnObject::ResetSN(bool isClean)
{
    if (!isClean)
        _sn = Global::GetInstance()->GenerateSN();
    else
        _sn = 0;
}
