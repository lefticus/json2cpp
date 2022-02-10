cmake_minimum_required(VERSION 3.16)

# Used as a tool to make sure that the standard used by tools
# and by the projects cannot get out of sync
set(USER_CXX_STANDARD 17)

# uncomment to set a default CXX standard for the external tools like clang-tidy and cppcheck
# and the targets that do not specify a standard.
# If not set, the latest supported standard for your compiler is used
# You can later set fine-grained standards for each target using `target_compile_features`
# Note: linking together projects compiled with different C++ standards may work, but
# it is not recommended because of possible issues with ABI
set(CMAKE_CXX_STANDARD ${USER_CXX_STANDARD})

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

# Add project_options v0.13.1
# https://github.com/cpp-best-practices/project_options
include(FetchContent)
FetchContent_Declare(
  _project_options
  URL https://github.com/lefticus/project_options/archive/refs/heads/make_cppcheck_flags_configurable.zip)
#                   URL https://github.com/cpp-best-practices/project_options/archive/refs/tags/v0.13.1.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

# uncomment to enable vcpkg:
# # Setup vcpkg - should be called before defining project()
# run_vcpkg()

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")
string(
  SUBSTRING "${GIT_SHA}"
            0
            8
            GIT_SHORT_SHA)

# Set the project name and language
project(
  json2cpp
  VERSION 0.0.1
  DESCRIPTION "Compiles JSON files into `static constexpr` C++ files"
  LANGUAGES CXX C)

if(GENERATOR_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
  # Make sure that all supported configuration types have their
  # associated conan packages available. You can reduce this
  # list to only the configuration types you use, but only if one
  # is not forced-set on the command line for VS
  set(CMAKE_CONFIGURATION_TYPES
      Debug
      Release
      RelWithDebInfo
      MinSizeRel)
endif()

include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

# defaulted_project_options sets recommended defaults and provides user and developer
# modes and full GUI support for choosing options at configure time

# for more flexibility, look into project_options() macro

# Any default can be overridden
# set(<feature_name>_DEFAULT <value>) - set default for both user and developer modes
# set(<feature_name>_DEVELOPER_DEFAULT <value>) - set default for developer mode
# set(<feature_name>_USER_DEFAULT <value>) - set default for user mode

# Initialize project_options variable related to this project
# This overwrites `project_options` and sets `project_warnings`
# uncomment the options to enable them:
dynamic_project_options(
  # Note: PCH is disabled by default in developer mode because these headers become
  # globally included and they can mask other errors
  PCH_HEADERS
  <vector>
  <string> # This is a list of headers to pre-compile, here are some common ones
  # CONAN_OPTIONS  # Extra options to pass to conan
  # MSVC_WARNINGS  # Override the defaults for the MSVC warnings
  # CLANG_WARNINGS # Override the defaults for the CLANG warnings
  # GCC_WARNINGS   # Override the defaults for the GCC warnings
  CPPCHECK_OPTIONS
  --enable=style,performance,warning,portability
  --inline-suppr
  # We cannot act on a bug/missing feature of cppcheck
  --suppress=internalAstError
  # if a file does not have an internalAstError, we get an unmatchedSuppression error
  --suppress=unmatchedSuppression
  --suppress=passedByValue
  --inconclusive)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9")
  target_link_libraries(project_options INTERFACE stdc++fs)
endif()

target_compile_features(project_options INTERFACE cxx_std_${USER_CXX_STANDARD})

option(ENABLE_LARGE_TESTS "Enable tests and tools that use the Energy+ schema file" ON)

# Adding the src:
add_subdirectory(src)

# Adding the tests:
option(ENABLE_TESTING "Enable the tests" ON)
if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests. Be sure to check out test/constexpr_tests for constexpr
testing")
  add_subdirectory(test)
endif()

option(ENABLE_FUZZING "Enable the fuzz tests" OFF)
if(ENABLE_FUZZING)
  message("Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
  add_subdirectory(fuzz_test)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_targets(all_targets)
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# set the startup project for the "play" button in MSVC
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT json2cpp)

set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)
