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

## CLI Usage

The json2cpp CLI can be used to convert a JSON file into a C++ source file that can be compiled into your project.

```shell
json2cpp version 0.0.1
Usage: ./build/src/Debug/json2cpp [OPTIONS] [<document_name>] [<input_file_name>] [<output_base_name>]

Positionals:
  <document_name> TEXT        The name of the document used in the generated C++ namespace and include guards
  <input_file_name> TEXT      The input JSON file to compile
  <output_base_name> TEXT     The base path for the output files. It will generate <output_base_name>.cpp and <output_base_name>.hpp

Options:
  -h,--help                   Print this help message and exit
  --version                   Show version information

```

For example, to compile a JSON file named `"./data/data.json"`:

```shell
json2cpp "data" "./data.json" "./data_source"
```
