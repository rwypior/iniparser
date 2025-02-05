cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_definitions(-DUNICODE -D_UNICODE)

project ("iniparser")
add_library(iniparser INTERFACE)
add_library(iniparser::iniparser ALIAS iniparser)

target_sources (iniparser PRIVATE "include/iniparser/iniparser.hpp")

target_include_directories (iniparser INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if(${BUILD_TESTING})
    message("Building unit tests")

    include(CTest)
    add_subdirectory ("tests")
endif()

# Install

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(ConfigPackageLocation lib/cmake/iniparser)

configure_package_config_file(
        cmake/iniparserConfig.cmake.in
	    "${CMAKE_CURRENT_BINARY_DIR}/iniparser/iniparserConfig.cmake"
	    INSTALL_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/iniparser"
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/iniparser/iniparserConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY AnyNewerVersion
)

install(
        TARGETS iniparser EXPORT iniparserTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        DIRECTORY "include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT iniparserTargets
        FILE iniparserTargets.cmake
        NAMESPACE iniparser::
        DESTINATION ${ConfigPackageLocation}
)

install(FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/iniparser/iniparserConfig.cmake" 
            "${CMAKE_CURRENT_BINARY_DIR}/iniparser/iniparserConfigVersion.cmake"
        DESTINATION ${ConfigPackageLocation}
)

export(PACKAGE iniparser)