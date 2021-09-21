cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0048 NEW)
project(my_gtest_pkgconfig VERSION 0.0.1 LANGUAGES CXX C)

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest)
#pkg_search_module(GMOCK REQUIRED gmock)

#add_definitions(-DUSE_HAL_DRIVER)
#add_definitions(-DSTM32F103xE)
add_definitions(-DBUILDCONFIG_TESTING_BUILD)
#add_definitions(-DDEBUG_ERR_CONSOLE_ON)
#add_definitions(-DDEBUG_WAR_CONSOLE_ON)
#add_definitions(-DTESTING_LOGGING)

set(CMAKE_CXX_STANDARD 17)

# -Wno-int-to-pointer-cast suppresses warnings from HAL code that wants to access registers
# this code isn't even used in the tests so nothing to worry

# -Wno-deprecated-declarations is neccessary as std::uncaught_exception (used within gtest)
# is deprecated in c++17 and has to be upgraded by google to not throw these errors
#set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations -Wno-int-to-pointer-cast")

include_directories(
    ../
    fake
    include
)

add_executable(testapp 
# testapp
src/main.cxx

# firmware
#../ParameterManager.cxx
../SettingsIO.cxx
../SettingsUser.cxx

# base


#  test
src/FakeMemoryTest.cxx
src/ParameterManagerTest.cxx
src/SettingsContainerTest.cxx
src/SettingsEntryArrayTest.cxx
src/SettingsEntryTest.cxx
src/SettingsIOTest.cxx
src/SettingsUserTest.cxx
)

target_link_libraries(testapp ${GTEST_LDFLAGS} ${GMOCK_LDFLAGS})
target_compile_options(testapp PUBLIC ${GTEST_CFLAGS} ${GMOCK_CFLAGS})

include(CTest)
add_test(first_and_only_test testapp)