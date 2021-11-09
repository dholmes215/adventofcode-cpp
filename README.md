# C++ Template
A demonstration of how to configure a modern C++ project properly.

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

Run `hello` (on Windows `hello.exe`) with no arguments:
```
> hello.exe
Hello World!
```

## License

This project is in the public domain (see `LICENSE`).
