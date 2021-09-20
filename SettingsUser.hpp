#pragma once

#include <array>
#include <cstdint>

/**
 * @brief Inherit from this class to automatically subscribe to
 * settings changes and get onSettingsUpdate() called
 *
 */

namespace settings
{
class SettingsUser
{
public:
    SettingsUser();
    virtual ~SettingsUser() = default;
    // TODO copy, move operator+constructorw

    /**
     * @brief User implemented function that queries relevant settings and save their own copy
     * Don't forget to call at least once to receive settings.
     *
     */
    virtual void onSettingsUpdate() = 0;

    /**
     * @brief Notifies all registered instances that settings changed
     *
     */
    static void notifySettingsUpdate();

private:
    static constexpr uint8_t MaxInstances = 16;
    static std::array<SettingsUser *, MaxInstances> registeredInstances;
    static uint8_t registeredInstancesCount;
};
} // namespace settings