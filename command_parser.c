#include "shell.h"

/**
 * is_executable - determines if a file is an executable command
 * @shell_info: the shell_info struct
 * @path: path to the file
 *
 * Return: 1 if true, 0 otherwise
 */
int is_executable(info_t *shell_info, char *path)
{
	struct stat st;

	(void)shell_info;
	if (!path || stat(path, &st))
		return (0);

	if (st.st_mode & S_IFREG)
	{
		return (1);
	}
	return (0);
}

/**
 * copy_chars - duplicates characters
 * @pathstr: the PATH string
 * @start: starting index
 * @stop: stopping index
 *
 * Return: pointer to new buffer
 */
char *copy_chars(char *pathstr, int start, int stop)
{
	char *buf = malloc((stop - start + 1 )* sizeof(char));
	int i = 0, k = 0;

	if (!buf)
		return (NULL);

	for (k = 0, i = start; i < stop; i++)
		if (pathstr[i] != ':')
			buf[k++] = pathstr[i];
	buf[k] = '\0';
	return (buf);
}

/**
 * find_path - finds this cmd in the PATH string
 * @shell_info: the shell_info struct
 * @pathstr: the PATH string
 * @cmd: the cmd to find
 *
 * Return: full path of cmd if found or NULL
 */
char *find_path(info_t *shell_info, char *pathstr, char *cmd)
{
	int i = 0, curr_pos = 0;
	char *path;

	if (!pathstr)
		return (NULL);
	if ((_strlen(cmd) > 2) && starts_with(cmd, "./"))
	{
		if (is_executable(shell_info, cmd))
			return (cmd);
	}
	while (1)
	{
		if (!pathstr[i] || pathstr[i] == ':')
		{
			path = copy_chars(pathstr, curr_pos, i);

			if (!path)
				return (NULL);

			if (!*path)
				_strcat(path, cmd);
			else
			{
				_strcat(path, "/");
				_strcat(path, cmd);
			}
			if (is_executable(shell_info, path))
				return (path);
			free(path);
			if (!pathstr[i])
				break;
			curr_pos = i;
		}
		i++;
	}
	return (NULL);
}
