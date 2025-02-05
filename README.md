iniparser is a simple header-only library for parsing .ini configuration files.
For usage samples, please refer to tests in `tests` directory.

Installation
=====
In order to use the library, either copy the `include/iniparser/iniparser.hpp`
to your project, or use CMake to install the library in your system:

```
git clone https://github.com/rwypior/iniparser.git
cd iniparser
mkdir build && cd build
cmake ..
sudo cmake --install .
```

Then in your project's CMakeLists.txt find and link the library as:

```
find_package(iniparser CONFIG)
target_link_libraries(your_target PRIVATE iniparser::iniparser)
```

And include the library in your sources:

```
#include <iniparser/iniparser.hpp>
```