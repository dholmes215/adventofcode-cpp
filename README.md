# Advent of Code in C++
David's solutions to Advent of Code problems in C++.

## Requirements

* **A C++20 compiler**
* **CMake 3.20.3 or later**

## Building

To configure the build system:

```
cmake -S [project_root_dir] -B [output_dir]
```

To build the project after configuring:

```
cmake --build [output_dir]
```

...or open the project using your CMake-compatible IDE or editor like **CLion**, **Visual Studio**, or **Visual Studio Code with the appropriate CMake extension**.

## Running

Run `runner` (on Windows `runner.exe`) with no arguments:
```
> runner.exe
Running solutions from Year 2015 Day 01 to Year 2015 Day 05 1 times
Day                  Solution                 Part A               Part B        Duration Iterations
Year 2015 Day 01     imperative                  138                 1771           8.4µs          1
Year 2015 Day 01     algorithm                   138                 1771          23.2µs          1
Year 2015 Day 01     ranges                      138                 1771           9.3µs          1
Year 2015 Day 02                             1606483              3842356            51µs          1
Year 2015 Day 03                                2572                 2631        1250.5µs          1
Year 2015 Day 04                              346386              9958218   1.56922e+06µs          1
Year 2015 Day 05                                 255                   55         443.1µs          1
```

TODO: Document command-line options.

## License

This project released under the Boost Software License 1.0 (see [LICENSE_1_0.txt](LICENSE_1_0.txt) for details).
