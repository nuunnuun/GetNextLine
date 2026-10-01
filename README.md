# get_next_line

A 42-school C project that reads a text stream one line at a time using `read()`, a configurable buffer, and persistent state between calls.

## Overview

```c
char *get_next_line(int fd);
```

Each successful call returns a newly allocated, null-terminated line. The newline is included when present. A final line without a newline is also returned. `NULL` indicates end of input or an error; this API does not distinguish the two. The caller must free every returned line.

## Build and run

Requires a C compiler and Make. The demo reads one file and prints its contents:

```sh
make
./gnl_demo README.md
make re BUFFER_SIZE=42
make fclean
```

Use `make re` when changing `BUFFER_SIZE`, because Make does not track command-line variable changes. The default is 1024 bytes. The Makefile builds a demonstration executable, not a required 42 library target.

To compile the function into another program:

```sh
cc -Wall -Wextra -Werror -DBUFFER_SIZE=42 \
    your_main.c get_next_line.c get_next_line_utils.c -o your_program
```

## Example usage

```c
int fd = open("input.txt", O_RDONLY);
if (fd < 0)
    return (1);
char *line = get_next_line(fd);
while (line)
{
    printf("%s", line);
    free(line);
    line = get_next_line(fd);
}
close(fd);
```

Include `<fcntl.h>`, `<stdio.h>`, and `"get_next_line.h"` for this example.

## Supported behavior

- Empty files, blank lines, and files with or without a final newline.
- Lines longer than `BUFFER_SIZE`.
- Repeated calls after EOF return `NULL`.
- Buffered complete lines are returned without another positive-length read.
- Read and allocation failures release the internal state.
- Nonpositive `BUFFER_SIZE` values return `NULL`.

This is the mandatory, single-stream implementation. It has one static remainder and does not support alternating file descriptors, concurrent calls, or binary strings containing embedded null bytes. Finish reading one stream before starting another. An invalid-fd call clears the shared state. Closing a descriptor alone does not release a saved remainder; stopping early can retain that allocation until cleanup or process exit. The join-based approach repeatedly copies accumulated text, so extremely long lines with small buffers are not performance-optimized.

## Structure

| File | Responsibility |
| --- | --- |
| `get_next_line.c` | Read chunks, extract a line, preserve the remainder |
| `get_next_line_utils.c` | String and allocation helpers |
| `get_next_line.h` | Public declaration, helper declarations, buffer configuration |
| `main.c` | File-reading demo |
| `tests/test_gnl.c` | Reference comparison, descriptor errors, pipe regression tests |
| `tests/run_tests.py` | Compile and execute the test matrix in temporary storage |

## Testing

```sh
make test
make norm
python3 tests/run_tests.py --sanitize
```

Tests require Python 3 and a POSIX environment. Norm checks require Norminette. The reference reader is POSIX `getline()`; it is used only in tests.

The matrix uses `BUFFER_SIZE` values 1, 2, 7, 42, 1024, and 65536. Cases include empty input, blank lines, final-newline variations, long lines, CRLF, repeated EOF, invalid descriptors, read errors, and pipes with buffered lines. Nonpositive buffer sizes are checked separately.

On Linux with GNU-compatible linker wrapping, allocation-failure injection and allocation accounting can also be run:

```sh
python3 tests/run_tests.py --fail-alloc
```

This optional flag is not portable to the macOS linker. AddressSanitizer leak detection may be unavailable inside restricted containers; see the accompanying validation report for what was actually checked.

## Concepts learned

File descriptors and stream offsets; `read()` and EOF; static state; heap ownership; null-terminated strings; error cleanup; configurable chunk sizes; regression tests and memory diagnostics.

## Status and author

Mandatory implementation cleaned up and regression-tested. Bonus multiple-fd support is not included. This repository is a learning project; validation is not a claim of official 42 evaluation or subject-version compliance.

Kullatida Raksanaves — [nuunnuun](https://github.com/nuunnuun)
