/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kraksana <kraksana@student.42bangkok.co>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 03:20:00 by kraksana          #+#    #+#             */
/*   Updated: 2026/10/01 03:20:00 by kraksana         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_free(char *res, char *chunk)
{
	char	*next;

	next = ft_strjoin(res, chunk);
	free(res);
	return (next);
}

static char	*ft_next(char *buffer)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i])
		i++;
	j = 0;
	while (buffer[i])
		buffer[j++] = buffer[i++];
	buffer[j] = '\0';
	if (j)
		return (buffer);
	free(buffer);
	return (NULL);
}

static char	*ft_line(char *buffer)
{
	char	*line;
	size_t	i;
	size_t	length;

	length = 0;
	while (buffer[length] && buffer[length] != '\n')
		length++;
	if (buffer[length] == '\n')
		length++;
	line = ft_calloc(length + 1, 1);
	if (!line)
		return (NULL);
	i = 0;
	while (i < length)
	{
		line[i] = buffer[i];
		i++;
	}
	return (line);
}

static char	*read_file(int fd, char *res)
{
	char	*chunk;
	ssize_t	bytes;

	chunk = ft_calloc((size_t)BUFFER_SIZE + 1, 1);
	bytes = 1;
	while (chunk && res && !ft_strchr(res, '\n') && bytes > 0)
	{
		bytes = read(fd, chunk, BUFFER_SIZE * (BUFFER_SIZE > 0));
		if (bytes < 0)
			break ;
		chunk[bytes] = '\0';
		if (bytes > 0)
			res = ft_free(res, chunk);
	}
	if (!chunk || bytes < 0)
	{
		free(res);
		res = NULL;
	}
	free(chunk);
	return (res);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, NULL, 0) < 0)
	{
		free(buffer);
		buffer = NULL;
		return (NULL);
	}
	if (!buffer)
		buffer = ft_calloc(1, 1);
	if (buffer && !ft_strchr(buffer, '\n'))
		buffer = read_file(fd, buffer);
	line = NULL;
	if (buffer && *buffer)
		line = ft_line(buffer);
	if (line)
		buffer = ft_next(buffer);
	else
	{
		free(buffer);
		buffer = NULL;
	}
	return (line);
}
