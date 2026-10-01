"""Compile in a temporary directory; compare every returned line with getline."""
import os
import pathlib
import subprocess
import tempfile
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
CC = os.environ.get('CC', 'cc')
cases = [b'', b'x', b'x\n', b'one\ntwo\nthree', b'one\ntwo\n',
         b'\n\n\n', b'a\n\nb\n', b'x' * 100000 + b'\nend',
         b'y' * 100000, b'\r\nhello\r\n', bytes(range(1, 128)) * 8]
sanitize = '--sanitize' in sys.argv
if sanitize:
    cases[7:9] = [b'x' * 20000 + b'\nend', b'y' * 20000]
with tempfile.TemporaryDirectory() as temp:
    temp = pathlib.Path(temp)
    for size in [1, 2, 7, 42, 1024, 65536]:
        executable = temp / 'test'
        flags = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g'] if sanitize else []
        if sanitize and sys.platform.startswith('linux'):
            flags += ['-no-pie']
        command = [CC, '-Wall', '-Wextra', '-Werror', '-I', str(ROOT),
                   f'-DBUFFER_SIZE={size}', *flags, str(ROOT / 'tests/test_gnl.c'),
                   str(ROOT / 'get_next_line.c'), str(ROOT / 'get_next_line_utils.c'),
                   '-o', str(executable)]
        if '--fail-alloc' in sys.argv:
            command += ['-DFAIL_ALLOC', '-Wl,--wrap=malloc', '-Wl,--wrap=free']
        subprocess.run(command, check=True)
        for data in cases:
            fixture = temp / 'input'
            fixture.write_bytes(data)
            subprocess.run([str(executable), str(fixture)], check=True, timeout=30)
        print(f'PASS BUFFER_SIZE={size}: {len(cases)} files, EOF, fd errors, pipe state')
    invalid = temp / 'invalid.c'
    invalid.write_text('#include \"get_next_line.h\"\n#include <assert.h>\nint main(void) { assert(get_next_line(0) == 0); return 0; }\n')
    for size in [0, -1]:
        subprocess.run([CC, '-Wall', '-Wextra', '-Werror', f'-DBUFFER_SIZE={size}',
                        '-I', str(ROOT), str(invalid), str(ROOT / 'get_next_line.c'),
                        str(ROOT / 'get_next_line_utils.c'), '-o', str(temp / 'invalid')], check=True)
        subprocess.run([str(temp / 'invalid')], check=True)
    print('PASS invalid BUFFER_SIZE compile and runtime checks (0, -1)')
