
#include "entity_system.h"
#include "test_entity.h"
#include "test_c_init.h"
#include "test_c_update.h"

int main()
{
    EntitySystem eSys;
    auto pEntity1 = eSys.CreateComponent<TestEntityWithInitAndUpdate>();
    auto pEntity2 = eSys.CreateComponent<TestEntityWithUpdate>();

    const auto pCInit = eSys.CreateComponent<TestCInit>();
    pEntity1->AddComponent(pCInit);

    const auto pCUpdate = eSys.CreateComponent<TestCUpdate>();
    pEntity2->AddComponent(pCUpdate);

    while (true)
    {
        eSys.Update();
    }
}
