# get_next_line

My implementation of `get_next_line` for 42 Bangkok. The function reads one line at a time from a file descriptor, keeping any leftover text for the next call.

This project helped me practice working with `read()`, static variables, and memory allocation in C.

## Usage

```c
char *get_next_line(int fd);
```

Each call returns an allocated string containing the next line, including `\n` if there is one. It returns `NULL` at the end of the input or if an error occurs. The caller must free each returned line.

For example, a file containing `Hello\nWorld` returns `"Hello\n"`, then `"World"`, then `NULL`.

## Build and run

A C compiler and Make are required. The included demo reads a file and prints its contents:

```sh
make
./gnl_demo README.md
make fclean
```

The default `BUFFER_SIZE` is 1024. To use a different value, rebuild:

```sh
make re BUFFER_SIZE=42
```

To use the function in another C program, include `get_next_line.h` and compile both source files with your program:

```sh
cc -Wall -Wextra -Werror -DBUFFER_SIZE=42 \
    your_main.c get_next_line.c get_next_line_utils.c -o your_program
```

## Supported behavior

- Empty files and blank lines
- Files with or without a final newline
- Lines longer than `BUFFER_SIZE`
- Repeated calls after EOF
- Cleanup after read or allocation errors

This version uses one static buffer. It supports reading one stream at a time; alternating between file descriptors is not included. Input is treated as text, without embedded null bytes.

Read until `NULL` before moving to another stream. If you stop early, closing the descriptor does not free the saved text. A call with `fd = -1` clears the internal buffer. Very long lines can also be slow with small buffers because each join copies the accumulated text.

## Project files

| File | Purpose |
| --- | --- |
| `get_next_line.c` | Reading, line extraction, and saved text |
| `get_next_line_utils.c` | String and allocation helpers |
| `get_next_line.h` | Declarations and buffer size |
| `main.c` | Example program |
| `tests/` | C tests and a Python script to run them |

## Testing

```sh
make test
make norm
```

The tests need Python 3; the implementation and demo are C. Norm checks need Norminette.

Tests cover empty input, newline variations, long lines, repeated EOF, descriptor errors, and pipes. They run with `BUFFER_SIZE` values 1, 2, 7, 42, 1024, and 65536, with separate checks for 0 and -1.

For additional memory checks on Linux:

```sh
python3 tests/run_tests.py --sanitize
python3 tests/run_tests.py --fail-alloc
```

The allocation-failure option requires GNU-compatible linker wrapping. ASan and UBSan passed on Linux. Functional tests and Norminette passed on macOS; ASan could not be checked there because its runtime timed out even with a separate test program.

## What I practiced

- File descriptors, `read()`, and EOF
- Keeping state between function calls
- Allocating, copying, and freeing strings
- Handling errors without losing allocated memory
- Testing edge cases and different buffer sizes

## Status

Mandatory implementation with a demo and tests. Multiple-file-descriptor bonus is not included.

## Author

Kullatida Raksanaves — [nuunnuun](https://github.com/nuunnuun)
