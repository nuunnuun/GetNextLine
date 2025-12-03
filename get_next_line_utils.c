#include "get_next_line.h"

char *ft_strjoin(const char *s1, const char *s2)
{
    char *res;
    int i;
    int j;

    i = 0;
    j = 0;

    res = (char *) malloc((ft_strlen(s1) + ft_strlen(s2) +1) * sizeof(char));
    
    if (!res)
        return(NULL); // At firsti use null and it doesnt work?
    while (s1[i])
        res[j++] = s1[i++];
    i = 0;
    while (s2[i])
        res[j++] = s2[i++];
    res[j] = 0;

    return (res);
}

size_t  ft_strlen(const char *str)
{
    int i;
    i = 0;
    while(str[i])
    {
        i++;
    }
    return(i);
}

char    *ft_strchr(const char *string, int findchar)
{
    char    *str;

    str = (char *)string;
    while (*str != findchar && *str != 0)
        str++;
    if (*str == findchar)
        return (str);
    else
        return (NULL);
}

void    ft_bzero(void *s, size_t n)
{
    char    *str;
    size_t  i;

    str = (char *)s;
    i = 0;
    while (i < n)
    {
        str[i] = '\0';
        i++;
    }
}

void    *ft_calloc(size_t elementcount, size_t elementsize)
{
    char    *res;

    res = malloc(elementcount * elementsize);
    if(!res)
        return(NULL);
    ft_bzero(res, elementcount * elementsize);
    return(res);
}
