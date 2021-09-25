#include "SettingsUser.hpp"
#include "SpecialAssert.hpp"

namespace settings
{
SettingsUser::SettingsUser()
{
    if (registeredInstancesCount == MaxInstances)
    {
        specialAssert(false); // maximum number of instances reached
    }
    index = registeredInstancesCount++;
    registeredInstances[index] = this;
}

SettingsUser::~SettingsUser()
{
    registeredInstances[index] = nullptr;
}

void SettingsUser::notifySettingsUpdate()
{
    for (uint8_t i = 0; i < registeredInstancesCount; ++i)
    {
        registeredInstances[i]->onSettingsUpdate();
    }
}

uint8_t SettingsUser::registeredInstancesCount = 0;
std::array<SettingsUser *, SettingsUser::MaxInstances> SettingsUser::registeredInstances = {
    nullptr};
} // namespace settings