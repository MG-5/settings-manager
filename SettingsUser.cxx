#include "SettingsUser.hpp"

namespace settings
{
SettingsUser::SettingsUser()
{
    if (registeredInstancesCount == MaxInstances)
    {
        configASSERT(0); // maximum number of instances reached
    }
    registeredInstances[registeredInstancesCount++] = this;
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