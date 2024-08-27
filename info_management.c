#include "shell.h"

/**
 * initialize_info - initializes info_t struct
 * @shell_info: struct address
 */
void initialize_info(info_t *shell_info)
{
	shell_info->arg = NULL;
	shell_info->argv = NULL;
	shell_info->path = NULL;
	shell_info->argc = 0;
}

/**
 * initialize_struct - sets up info_t struct
 * @shell_info: struct address
 * @av: argument vector
 */
void initialize_struct(info_t *shell_info, char **av)
{
	int index = 0;

	shell_info->fname = av[0];
	if (shell_info->arg)
	{
		shell_info->argv = split_string(shell_info->arg, " \t");
		if (!shell_info->argv)
		{
			shell_info->argv = malloc(sizeof(char *) * 2);
			if (shell_info->argv)
			{
				shell_info->argv[0] = _strdup(shell_info->arg);
				shell_info->argv[1] = NULL;
			}
		}
		for (index = 0; shell_info->argv && shell_info->argv[index]; index++)
			;
		shell_info->argc = index;

		replace_alias(shell_info);
		replace_vars(shell_info);
	}
}

/**
 * release_info - frees info_t struct fields
 * @shell_info: struct address
 * @all: true if freeing all fields
 */
void release_info(info_t *shell_info, int all)
{
	ffree(shell_info->argv);
	shell_info->argv = NULL;
	shell_info->path = NULL;
	if (all)
	{
		if (!shell_info->cmd_buf)
			free(shell_info->arg);
		if (shell_info->env)
			free_list(&(shell_info->env));
		if (shell_info->history)
			free_list(&(shell_info->history));
		if (shell_info->alias)
			free_list(&(shell_info->alias));
		ffree(shell_info->environ);
		shell_info->environ = NULL;
		free_pointer((void **)shell_info->cmd_buf);
		if (shell_info->readfd > 2)
			close(shell_info->readfd);
		_putchar(BUF_FLUSH);
	}
}
