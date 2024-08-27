#include "shell.h"

/**
 * fetch_history_file - gets the history file
 * @shell_info: parameter struct
 *
 * Return: allocated string containing history file
 */
char *fetch_history_file(info_t *shell_info)
{
	char *buffer, *directory;

	directory = get_env(shell_info, "HOME=");
	if (!directory)
		return (NULL);
	buffer = malloc(sizeof(char) * (_strlen(directory) + _strlen(HIST_FILE) + 2));
	if (!buffer)
		return (NULL);
	buffer[0] = 0;
	_strcpy(buffer, directory);
	_strcat(buffer, "/");
	_strcat(buffer, HIST_FILE);
	return (buffer);
}

/**
 * save_history - creates a file, or appends to an existing file
 * @shell_info: the parameter struct
 *
 * Return: 1 on success, else -1
 */
int save_history(info_t *shell_info)
{
	ssize_t file_descriptor;
	char *filename = fetch_history_file(shell_info);
	list_t *node = NULL;

	if (!filename)
		return (-1);

	file_descriptor = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
	free(filename);
	if (file_descriptor == -1)
		return (-1);
	for (node = shell_info->history; node; node = node->next)
	{
		print_string_to_fd(node->str, file_descriptor);
		_write_char_to_fd('\n', file_descriptor);
	}
	_write_char_to_fd(BUF_FLUSH, file_descriptor);
	close(file_descriptor);
	return (1);
}

/**
 * load_history - reads history from file
 * @shell_info: the parameter struct
 *
 * Return: histcount on success, 0 otherwise
 */
int load_history(info_t *shell_info)
{
	int i, last = 0, linecount = 0;
	ssize_t file_descriptor, read_length, file_size = 0;
	struct stat st;
	char *buffer = NULL, *filename = fetch_history_file(shell_info);

	if (!filename)
		return (0);

	file_descriptor = open(filename, O_RDONLY);
	free(filename);
	if (file_descriptor == -1)
		return (0);
	if (!fstat(file_descriptor, &st))
		file_size = st.st_size;
	if (file_size < 2)
		return (0);
	buffer = malloc(sizeof(char) * (file_size + 1));
	if (!buffer)
		return (0);
	read_length = read(file_descriptor, buffer, file_size);
	buffer[file_size] = 0;
	if (read_length <= 0)
		return (free(buffer), 0);
	close(file_descriptor);
	for (i = 0; i < file_size; i++)
		if (buffer[i] == '\n')
		{
			buffer[i] = 0;
			append_history_list(shell_info, buffer + last, linecount++);
			last = i + 1;
		}
	if (last != i)
		append_history_list(shell_info, buffer + last, linecount++);
	free(buffer);
	shell_info->histcount = linecount;
	while (shell_info->histcount-- >= HIST_MAX)
		delete_node_at_index(&(shell_info->history), 0);
	update_history_numbers(shell_info);
	return (shell_info->histcount);
}

/**
 * append_history_list - adds entry to a history linked list
 * @shell_info: Structure containing potential arguments. Used to maintain
 * @buffer: buffer
 * @linecount: the history linecount, histcount
 *
 * Return: Always 0
 */
int append_history_list(info_t *shell_info, char *buffer, int linecount)
{
	list_t *node = NULL;

	if (shell_info->history)
		node = shell_info->history;
	add_node_end(&node, buffer, linecount);

	if (!shell_info->history)
		shell_info->history = node;
	return (0);
}

/**
 * update_history_numbers - renumbers the history linked list after changes
 * @shell_info: Structure containing potential arguments. Used to maintain
 *
 * Return: the new histcount
 */
int update_history_numbers(info_t *shell_info)
{
	list_t *node = shell_info->history;
	int i = 0;

	while (node)
	{
		node->num = i++;
		node = node->next;
	}
	return (shell_info->histcount = i);
}
