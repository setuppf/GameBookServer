#include "sn_object.h"
#include "global.h"

SnObject::SnObject() {
    _sn = Global::GetInstance()->GenerateSN();
}

SnObject::SnObject(uint64 sn) {
    _sn = sn;
}

uint64 SnObject::GetSN() const {
    return _sn;
}

void SnObject::ResetSN()
{
    _sn = Global::GetInstance()->GenerateSN();
}
