[![Langulus::Logger CI](https://github.com/Langulus/Logger/actions/workflows/ci.yml/badge.svg)](https://github.com/Langulus/Logger/actions/workflows/ci.yml)
# Langulus::Logger

A simple C++20 logger, based on the [{fmt} library](https://github.com/fmtlib/fmt).
It supports tabulation, colors, and external attachments, and is part of the [Langulus](https://langulus.com) framework.

![Visual demonstation](https://github.com/Langulus/Logger/blob/main/demo.png?raw=true)

# Usage
### Write line
You can use any of the predefined line functions, each with its distinct style:
```c++20
using namespace Langulus;
Logger::Line("Write a plain text line with the currently set style");
Logger::Fatal("Write fatal error with error number: ", 666);
Logger::Warning("Write a complex", " warning ", Logger::Red, "with all kinds of arguments: ", '1', 1);
Logger::Error(...);
Logger::Warning(...);
Logger::Verbose(...);
Logger::Info(...);
Logger::Message(...);
Logger::Special(...);
Logger::Flow(...);
Logger::Input(...);
Logger::Network(...);
Logger::OS(...);
Logger::Prompt(...);
```
#WIP
