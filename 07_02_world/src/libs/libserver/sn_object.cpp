#include "sn_object.h"
#include "global.h"

SnObject::SnObject() {
    _sn = Global::GetInstance()->GenerateSN();
}

uint64 SnObject::GetSN() const {
    return _sn;
}

void SnObject::SetSN(uint64 sn)
{
    _sn = sn;
}
