#pragma once

#include "common.h"
#include "global.h"

class SnObject {
public:
    virtual ~SnObject() {}

    SnObject( ) {
        _sn = Global::GetInstance( )->GenerateSN( );
    }

    SnObject( uint64 sn ) {
        _sn = sn;
    }

    uint64 GetSN( ) const {
        return _sn;
    }

protected:
    uint64 _sn;
};