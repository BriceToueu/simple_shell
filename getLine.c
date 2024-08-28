#include "shell.h"

/**
 * input_buffer - buffers chained commands
 * @shell_info: parameter struct
 * @buf: address of buffer
 * @len: address of len var
 *
 * Return: bytes read
 */
ssize_t input_buffer(info_t *shell_info, char **buf, size_t *len)
{
	ssize_t bytes_read = 0;
	size_t buffer_length = 0;

	if (!*len) /* if nothing left in the buffer, fill it */
	{
		free(*buf);
		*buf = NULL;
		signal(SIGINT, sigintHandler);
/* #if USE_GETLINE */
		/* bytes_read = getline(buf, &buffer_length, stdin); */
/* #else */
		bytes_read = _getline(shell_info, buf, &buffer_length);
/* #endif */
		if (bytes_read > 0)
		{
			if ((*buf)[bytes_read - 1] == '\n')
			{
				(*buf)[bytes_read - 1] = '\0'; /* remove trailing newline */
				bytes_read--;
			}
			shell_info->linecount_flag = 1;
			strip_comments(*buf);
			append_history_list(shell_info, *buf, shell_info->histcount++);
			/* *len = bytes_read;
			shell_info->cmd_buf = buf; */
		}
	}
	return (bytes_read);
}

/**
 * get_input - gets a line minus the newline
 * @shell_info: parameter struct
 *
 * Return: bytes read
 */
ssize_t get_input(info_t *shell_info)
{
	static char *buffer; /* the ';' command chain buffer */
	static size_t i, j, len;
	ssize_t bytes_read = 0;
	char **buffer_ptr = &(shell_info->arg), *current_position;

	_putchar(BUF_FLUSH);
	bytes_read = input_buffer(shell_info, &buffer, &len);
	if (bytes_read == -1) /* EOF */
		return (-1);
	if (len) /* we have commands left in the chain buffer */
	{
		j = i; /* init new iterator to current buf position */
		current_position = buffer + i; /* get pointer for return */

		check_chain(shell_info, buffer, &j, i, len);
		while (j < len) /* iterate to semicolon or end */
		{
			if (is_chain(shell_info, buffer, &j))
				break;
			j++;
		}

		i = j + 1; /* increment past nulled ';' */
		if (i >= len) /* reached end of buffer? */
		{
			i = len = 0; /* reset position and length */
			shell_info->cmd_buf_type = CMD_NORM;
		}

		/* pass back pointer to current command position */
		*buffer_ptr = current_position;
		/* return length of current command */
		return (_strlen(current_position));
	}

	/* else not a chain, pass back buffer from _getline() */
	*buffer_ptr = buffer;
	return (bytes_read); /* return length of buffer from _getline() */
}

/**
 * read_buffer - reads a buffer
 * @shell_info: parameter struct
 * @buf: buffer
 * @i: size
 *
 * Return: bytes read
 */
ssize_t read_buffer(info_t *shell_info, char *buf, size_t *i)
{
	ssize_t bytes_read = 0;

	if (*i)
		return (0);
	bytes_read = read(shell_info->readfd, buf, READ_BUF_SIZE);
	if (bytes_read >= 0)
		*i = bytes_read;
	return (bytes_read);
}

/**
 * _getline - gets the next line of input from STDIN
 * @shell_info: parameter struct
 * @ptr: address of pointer to buffer, preallocated or NULL
 * @length: size of preallocated ptr buffer if not NULL
 *
 * Return: bytes read
 */
int _getline(info_t *shell_info, char **ptr, size_t *length)
{
	static char buf[READ_BUF_SIZE];
	static size_t i, len;
	size_t k;
	ssize_t bytes_read = 0, total_bytes = 0;
	char *p = NULL, *new_p = NULL, *newline_pos;

	p = *ptr;
	if (p && length)
		total_bytes = *length;
	if (i == len)
		i = len = 0;

	bytes_read = read_buffer(shell_info, buf, &len);
	if (bytes_read == -1 || (bytes_read == 0 && len == 0))
		return (-1);

	newline_pos = _strchr(buf + i, '\n');
	k = newline_pos ? 1 + (unsigned int)(newline_pos - buf) : len;
	new_p = _realloc(p, total_bytes, total_bytes ? total_bytes + k : k + 1);
	if (!new_p) /* MALLOC FAILURE! */
		return (p ? free(p), -1 : -1);

	if (total_bytes)
		_strncat(new_p, buf + i, k - i);
	else
		_strncpy(new_p, buf + i, k - i + 1);

	total_bytes += k - i;
	i = k;
	p = new_p;

	if (length)
		*length = total_bytes;
	*ptr = p;
	return (total_bytes);
}

/**
 * sigintHandler - blocks ctrl-C
 * @sig_num: the signal number
 *
 * Return: void
 */
void sigintHandler(__attribute__((unused))int sig_num)
{
	_puts("\n");
	_puts("$ ");
	_putchar(BUF_FLUSH);
}
