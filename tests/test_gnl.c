#define _POSIX_C_SOURCE 200809L
#include "get_next_line.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#ifdef FAIL_ALLOC
static long calls;
static long fail_at;
static void *live[4096];
static int outstanding;
void *__real_malloc(size_t size);
void __real_free(void *ptr);
void __wrap_free(void *ptr)
{
	for (int i = 0; i < 4096; i++)
		if (ptr && live[i] == ptr)
		{
			live[i] = NULL;
			outstanding--;
			break;
		}
	__real_free(ptr);
}
void *__wrap_malloc(size_t size)
{
	calls++;
	if (calls == fail_at)
		return (NULL);
	void *ptr = __real_malloc(size);
	if (ptr)
	{
		for (int i = 0; i < 4096; i++)
			if (!live[i])
			{
				live[i] = ptr;
				outstanding++;
				return (ptr);
			}
		assert(0);
	}
	return (ptr);
}
#endif

int main(int argc, char **argv)
{
	FILE *reference;
	char *expected = NULL;
	size_t capacity = 0;
	ssize_t length;
	char *line;
	int fd;
	int pipes[2];

	assert(argc == 2);
#ifdef FAIL_ALLOC
	for (fail_at = 1; fail_at < 40; fail_at++)
	{
		calls = 0;
		fd = open(argv[1], O_RDONLY);
		assert(fd >= 0);
		while ((line = get_next_line(fd)))
			free(line);
		close(fd);
		free(get_next_line(-1));
		assert(outstanding == 0);
	}
	fail_at = 0;
#endif
	fd = open(argv[1], O_RDONLY);
	reference = fopen(argv[1], "r");
	assert(fd >= 0 && reference);
	while ((length = getline(&expected, &capacity, reference)) >= 0)
	{
		line = get_next_line(fd);
		assert(line && strlen(line) == (size_t)length);
		assert(memcmp(line, expected, (size_t)length + 1) == 0);
		free(line);
	}
	for (int i = 0; i < 5; i++)
		assert(get_next_line(fd) == NULL);
	free(expected);
	fclose(reference);
	close(fd);
	assert(get_next_line(fd) == NULL);
	assert(get_next_line(-1) == NULL);
	fd = open(".", O_RDONLY);
	assert(fd >= 0);
	assert(get_next_line(fd) == NULL);
	close(fd);
	assert(pipe(pipes) == 0);
	assert(write(pipes[1], "A\nB\n", 4) == 4);
	alarm(3);
	line = get_next_line(pipes[0]);
	assert(line && strcmp(line, "A\n") == 0);
	free(line);
	line = get_next_line(pipes[0]);
	assert(line && strcmp(line, "B\n") == 0);
	free(line);
	alarm(0);
	close(pipes[1]);
	assert(get_next_line(pipes[0]) == NULL);
	close(pipes[0]);
	assert(pipe(pipes) == 0);
	assert(write(pipes[1], "A\nB\n", 4) == 4);
	line = get_next_line(pipes[0]);
	assert(line);
	free(line);
	close(pipes[0]);
	close(pipes[1]);
	assert(get_next_line(pipes[0]) == NULL);

#ifdef FAIL_ALLOC
	assert(outstanding == 0);
#endif
	return (0);
}
