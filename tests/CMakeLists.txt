cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("iniparserTest")

add_executable(iniparserTest)
target_sources(iniparserTest PRIVATE "test.cpp" "test2.cpp")
add_dependencies(iniparserTest iniparser)

set_property(TARGET iniparserTest PROPERTY CXX_STANDARD 17)

target_compile_definitions(iniparserTest
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

find_package(Catch2 3.6.0 REQUIRED)

target_include_directories(iniparserTest PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(iniparserTest PRIVATE iniparser)
target_link_libraries(iniparserTest PRIVATE Catch2::Catch2WithMain)

add_custom_command(
  TARGET iniparserTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${PROJECT_SOURCE_DIR}/resources" "${PROJECT_BINARY_DIR}/resources"
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(iniparserTest)
