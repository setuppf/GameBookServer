#include "object_block.h"
#include "object_pool_interface.h"

ObjectBlock::ObjectBlock(IDynamicObjectPool* pPool) {
    _pPool = pPool;
}

ObjectBlock::~ObjectBlock() {
    _pPool = nullptr;
}

void ObjectBlock::Dispose()
{
    BackToPool();
}
