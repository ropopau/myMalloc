# Malloc using Bucket algorithm

This small project is purely educative. It is just a reimplementation of malloc(3).
I choose the bucket algorithm. 

## How to

To use malloc in your C sourcefile, you must compile this project as a dynamic library.
Then you should explicitely set the environment variable LD_LIBRARY_PATH (DYLD_LIBRARY_PATH in Mac os) to the compiled library's directory.

(Although in my case, I had to use the rpath flag method, due to Mac OS security)

```shell
make # compile the library and the test source file 

make sanitize # compile the library and the test source files with memory sanitizing flags
make check # Execute the tests
make clean # clean every artifacts

```
The only visible symbols are malloc, realloc, calloc and free.
