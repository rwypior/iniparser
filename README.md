iniparser
==========

Simple single-header INI file parser library. The only thing that needs to be done to use this library is to just include `.iniparser.hpp` file.

The library contains `Ini::Parser` class that contains two public methods: 

Parse
---
```C++
parse(const std::string &source, Model &model)
```
Which parses given INI source text and put the result into specified model reference.

readFile
---
```C++
readFile(const std::string &path, Model &model)
```
An utility method that reads the specified file and then uses `parse` method with the contents of the file.

---

Both functions return `Ini::Result` object that contains result code and optionally error description.

Simple use case
---
Examples are included in `example.cpp` file as well as in test project. The simplest usage of the library could look like follows:
```C++
Ini::Model model;
Ini::Result result = Ini::Parser().readFile("../resources/example.ini", model);
std::string data = model.sections["section"]["var"]->getValue();
```
