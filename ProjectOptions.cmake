include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(json2cpp_setup_options)
  option(json2cpp_ENABLE_LARGE_TESTS ON)

  option(json2cpp_ENABLE_HARDENING "Enable hardening" ON)
  option(json2cpp_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    json2cpp_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    json2cpp_ENABLE_HARDENING
    OFF)


  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()

  json2cpp_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  option(json2cpp_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${LIBFUZZER_SUPPORTED})


  if(NOT PROJECT_IS_TOP_LEVEL OR json2cpp_PACKAGING_MAINTAINER_MODE)
    option(json2cpp_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(json2cpp_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(json2cpp_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(json2cpp_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(json2cpp_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(json2cpp_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(json2cpp_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(json2cpp_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(json2cpp_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(json2cpp_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(json2cpp_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(json2cpp_ENABLE_PCH "Enable precompiled headers" OFF)
    option(json2cpp_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(json2cpp_ENABLE_IPO "Enable IPO/LTO" ON)
    option(json2cpp_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(json2cpp_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(json2cpp_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(json2cpp_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(json2cpp_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(json2cpp_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(json2cpp_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(json2cpp_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(json2cpp_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(json2cpp_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(json2cpp_ENABLE_PCH "Enable precompiled headers" OFF)
    option(json2cpp_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      json2cpp_ENABLE_IPO
      json2cpp_WARNINGS_AS_ERRORS
      json2cpp_ENABLE_USER_LINKER
      json2cpp_ENABLE_SANITIZER_ADDRESS
      json2cpp_ENABLE_SANITIZER_LEAK
      json2cpp_ENABLE_SANITIZER_UNDEFINED
      json2cpp_ENABLE_SANITIZER_THREAD
      json2cpp_ENABLE_SANITIZER_MEMORY
      json2cpp_ENABLE_UNITY_BUILD
      json2cpp_ENABLE_CLANG_TIDY
      json2cpp_ENABLE_CPPCHECK
      json2cpp_ENABLE_COVERAGE
      json2cpp_ENABLE_PCH
      json2cpp_ENABLE_CACHE)
  endif()
endmacro()

macro(json2cpp_global_options)
  if(json2cpp_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    json2cpp_enable_ipo()
  endif()

  if(json2cpp_ENABLE_HARDENING AND json2cpp_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    set(ENABLE_UBSAN_MINIMAL_RUNTIME NOT json2cpp_ENABLE_SANITIZER_UNDEFINED)
    json2cpp_enable_hardening(json2cpp_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(json2cpp_local_options)
  if (PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(json2cpp_warnings INTERFACE)
  add_library(json2cpp_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  json2cpp_set_project_warnings(
    json2cpp_warnings
    ${json2cpp_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(json2cpp_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(json2cpp_options)
  endif()

  include(cmake/Sanitizers.cmake)
  json2cpp_enable_sanitizers(
    json2cpp_options
    ${json2cpp_ENABLE_SANITIZER_ADDRESS}
    ${json2cpp_ENABLE_SANITIZER_LEAK}
    ${json2cpp_ENABLE_SANITIZER_UNDEFINED}
    ${json2cpp_ENABLE_SANITIZER_THREAD}
    ${json2cpp_ENABLE_SANITIZER_MEMORY})

  set_target_properties(json2cpp_options PROPERTIES UNITY_BUILD ${json2cpp_ENABLE_UNITY_BUILD})

  if(json2cpp_ENABLE_PCH)
    target_precompile_headers(
      json2cpp_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(json2cpp_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    json2cpp_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(json2cpp_ENABLE_CLANG_TIDY)
    json2cpp_enable_clang_tidy(json2cpp_options ${json2cpp_WARNINGS_AS_ERRORS})
  endif()

  if(json2cpp_ENABLE_CPPCHECK)
    json2cpp_enable_cppcheck(${json2cpp_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(json2cpp_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    json2cpp_enable_coverage(json2cpp_options)
  endif()

  if(json2cpp_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(json2cpp_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(json2cpp_ENABLE_HARDENING AND NOT json2cpp_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    set(ENABLE_UBSAN_MINIMAL_RUNTIME NOT json2cpp_ENABLE_SANITIZER_UNDEFINED)
    json2cpp_enable_hardening(json2cpp_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
