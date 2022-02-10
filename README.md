# json2cpp

![CI](https://github.com/lefticus/json2cpp/workflows/ci/badge.svg)
[![codecov](https://codecov.io/gh/lefticus/json2cpp/branch/main/graph/badge.svg)](https://codecov.io/gh/lefticus/json2cpp)
[![Language grade: C++](https://img.shields.io/lgtm/grade/cpp/github/lefticus/json2cpp)](https://lgtm.com/projects/g/lefticus/json2cpp/context:cpp)

**json2cpp** compiles a json file into `static constexpr` data structures that can be used at compile time or runtime.

Features

 * Literally 0 runtime overhead for loading the statically compiled JSON resource
 * Fully constexpr capable if you want to make compile-time decisions based on the JSON resource file
 * A `.cpp` firewall file is provided for you, if you have a large resource and don't want to pay the cost of compiling it more than once (but for normal size files it is VERY fast to compile, they are just data structures)
 * [nlohmann::json](https://github.com/nlohmann/json) compatible API (should be a drop-in replacement, some features might still be missing)
 * [valijson](https://github.com/tristanpenman/valijson) adapter file provided


See the [test](test) folder for examples for building resources, using the valijson adapter, constexpr usage of resources, and firewalled usage of resources.
