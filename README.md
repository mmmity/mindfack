# minDFAck
A set of tools for working with finite automatons and regular expressions.

## Build
To build, run command 
`mkdir build && cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ && cmake --build build --target <your_target>`

(may be stuck on "Configuring build-in curl.." for a while)

## Running tests
To run tests, build target `coverage-report`. To run project, build target `mindfack`