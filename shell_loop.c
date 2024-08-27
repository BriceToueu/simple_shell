#include "shell.h"

/**
 * hsh - main shell loop
 * @shell_info: the parameter & return info struct
 * @av: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int hsh(info_t *shell_info, char **av)
{
	ssize_t r = 0;
	int builtin_ret = 0;

	while (r != -1 && builtin_ret != -2)
	{
		initialize_info(shell_info);
		if (is_interactive_mode(shell_info))
			_puts("$ ");
		_write_char_to_stderr(BUF_FLUSH);
		r = get_input(shell_info);
		if (r != -1)
		{
			initialize_struct(shell_info, av);
			builtin_ret = find_builtin(shell_info);
			if (builtin_ret == -1)
				find_executable(shell_info);
		}
		else if (is_interactive_mode(shell_info))
			_putchar('\n');
		release_info(shell_info, 0);
	}
	save_history(shell_info);
	release_info(shell_info, 1);
	if (!is_interactive_mode(shell_info) && shell_info->status)
		exit(shell_info->status);
	if (builtin_ret == -2)
	{
		if (shell_info->err_num == -1)
			exit(shell_info->status);
		exit(shell_info->err_num);
	}
	return (builtin_ret);
}

/**
 * find_builtin - finds a builtin command
 * @shell_info: the parameter & return info struct
 *
 * Return: -1 if builtin not found,
 * 0 if builtin executed successfully,
 * 1 if builtin found but not successful,
 * 2 if builtin signals exit()
 */
int find_builtin(info_t *shell_info)
{
	int i, built_in_ret = -1;
	builtin_table builtintbl[] = {
		{"exit", exit_shell},
		{"env", print_environment},
		{"help", display_help},
		{"history", display_history},
		{"setenv", set_environment_variable},
		{"unsetenv", unset_environment_variable},
		{"cd", change_directory},
		{"alias", mimic_alias},
		{NULL, NULL}
	};

	for (i = 0; builtintbl[i].type; i++)
		if (_strcmp(shell_info->argv[0], builtintbl[i].type) == 0)
		{
			shell_info->line_count++;
			built_in_ret = builtintbl[i].func(shell_info);
			break;
		}
	return (built_in_ret);
}

/**
 * find_executable - finds a command in PATH
 * @shell_info: the parameter & return info struct
 *
 * Return: void
 */
void find_executable(info_t *shell_info)
{
	char *path = NULL;
	int i, k;

	shell_info->path = shell_info->argv[0];
	if (shell_info->linecount_flag == 1)
	{
		shell_info->line_count++;
		shell_info->linecount_flag = 0;
	}
	for (i = 0, k = 0; shell_info->arg[i]; i++)
		if (!is_character_delim(shell_info->arg[i], " \t\n"))
			k++;
	if (!k)
		return;

	path = find_path(
			shell_info, get_env(shell_info, "PATH="), shell_info->argv[0]);
	if (path)
	{
		shell_info->path = path;
		fork_executable(shell_info);
	}
	else
	{
		if ((is_interactive_mode(shell_info) || get_env(shell_info, "PATH=")
					|| shell_info->argv[0][0] == '/') && is_executable(
						shell_info, shell_info->argv[0]))
			fork_executable(shell_info);
		else if (*(shell_info->arg) != '\n')
		{
			shell_info->status = 127;
			print_error(shell_info, "not found\n");
		}
	}
}

/**
 * fork_executable - forks a an exec thread to run cmd
 * @shell_info: the parameter & return info struct
 *
 * Return: void
 */
void fork_executable(info_t *shell_info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
		/* TODO: PUT ERROR FUNCTION */
		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(
				shell_info->path, shell_info->argv, get_environ(shell_info))
				== -1)
		{
			release_info(shell_info, 1);
			if (errno == EACCES)
				exit(126);
			exit(1);
		}
		/* TODO: PUT ERROR FUNCTION */
	}
	else
	{
		wait(&(shell_info->status));
		if (WIFEXITED(shell_info->status))
		{
			shell_info->status = WEXITSTATUS(shell_info->status);
			if (shell_info->status == 126)
				print_error(shell_info, "Permission denied\n");
		}
	}
}
