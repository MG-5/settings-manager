cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0048 NEW)
project(my_gtest_pkgconfig VERSION 0.0.1 LANGUAGES CXX C)

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ../
        fake
        include
)

add_executable(testapp
        # testapp
        src/main.cxx

        # firmware
        ../SettingsIO.cxx
        ../SettingsUser.cxx

        #  test
        src/FakeMemoryTest.cxx
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