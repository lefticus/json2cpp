include(CMakeFindDependencyMacro)

find_dependency(ValiJSON CONFIG)

include(${CMAKE_CURRENT_LIST_DIR}/json2cppTargets.cmake)