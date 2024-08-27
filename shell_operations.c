#include "shell.h"

/**
 * exit_shell - exits the shell
 * @shell_info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: exits with a given exit status
 * (0) if shell_info.argv[0] != "exit"
 */
int exit_shell(info_t *shell_info)
{
	int exitcheck;

	if (shell_info->argv[1]) /* If there is an exit argument */
	{
		exitcheck = str_to_int(shell_info->argv[1]);
		if (exitcheck == -1)
		{
			shell_info->status = 2;
			print_error(shell_info, "Illegal number: ");
			_print_in_str(shell_info->argv[1]);
			_write_char_to_stderr('\n');
			return (1);
		}
		shell_info->err_num = str_to_int(shell_info->argv[1]);
		return (-2);
	}
	shell_info->err_num = -1;
	return (-2);
}

/**
 * change_directory - changes the current directory of the process
 * @shell_info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: Always 0
 */
int change_directory(info_t *shell_info)
{
	char *s, *dir, buffer[1024];
	int chdir_ret;

	s = getcwd(buffer, 1024);
	if (!s)
		_puts("TODO: >>getcwd failure emsg here<<\n");
	if (!shell_info->argv[1])
	{
		dir = get_env(shell_info, "HOME=");
		if (!dir)
			chdir_ret = /* TODO: what should this be? */
				chdir((dir = get_env(shell_info, "PWD=")) ? dir : "/");
		else
			chdir_ret = chdir(dir);
	}
	else if (_strcmp(shell_info->argv[1], "-") == 0)
	{
		if (!get_env(shell_info, "OLDPWD="))
		{
			_puts(s);
			_putchar('\n');
			return (1);
		}
		_puts(get_env(shell_info, "OLDPWD=")), _putchar('\n');
		chdir_ret = /* TODO: what should this be? */
			chdir((dir = get_env(shell_info, "OLDPWD=")) ? dir : "/");
	}
	else
		chdir_ret = chdir(shell_info->argv[1]);
	if (chdir_ret == -1)
	{
		print_error(shell_info, "can't cd to ");
		_print_in_str(shell_info->argv[1]), _write_char_to_stderr('\n');
	}
	else
	{
		_setenv(shell_info, "OLDPWD", get_env(shell_info, "PWD="));
		_setenv(shell_info, "PWD", getcwd(buffer, 1024));
	}
	return (0);
}

/**
 * display_help - displays help information
 * @shell_info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: Always 0
 */
int display_help(info_t *shell_info)
{
	char **arg_array;

	arg_array = shell_info->argv;
	_puts("help call works. Function not yet implemented \n");
	if (0)
		_puts(*arg_array); /* temp att_unused workaround */
	return (0);
}
