#include "shell.h"

/**
 * print_environment - prints the current environment
 * @shell_info: Structure containing potential arguments. Used to maintain
 *          constant function prototype.
 * Return: Always 0
 */
int print_environment(info_t *shell_info)
{
	print_list_str(shell_info->env);
	return (0);
}

/**
 * get_env - gets the value of an environ variable
 * @shell_info: Structure containing potential arguments. Used to maintain
 * @name: env var name
 *
 * Return: the value
 */
char *get_env(info_t *shell_info, const char *name)
{
	list_t *node = shell_info->env;
	char *p;

	while (node)
	{
		p = starts_with(node->str, name);
		if (p && *p)
			return (p);
		node = node->next;
	}
	return (NULL);
}

/**
 * set_environment_variable - Initialize a new environment variable,
 *                            or modify an existing one
 * @shell_info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 *  Return: Always 0
 */
int set_environment_variable(info_t *shell_info)
{
	if (shell_info->argc != 3)
	{
		_print_in_str("Too many arguments\n");
		return (1);
	}
	if (_setenv(shell_info, shell_info->argv[1], shell_info->argv[2]))
		return (0);
	return (1);
}

/**
 * unset_environment_variable - Remove an environment variable
 * @shell_info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 * Return: Always 0
 */
int unset_environment_variable(info_t *shell_info)
{
	int i;

	if (shell_info->argc == 1)
	{
		_print_in_str("Too few arguments.\n");
		return (1);
	}
	for (i = 1; i <= shell_info->argc; i++)
		_unsetenv(shell_info, shell_info->argv[i]);

	return (0);
}

/**
 * fill_env_list - populates env linked list
 * @shell_info: Structure containing potential arguments. Used to maintain
 *          constant function prototype.
 * Return: Always 0
 */
int fill_env_list(info_t *shell_info)
{
	list_t *node = NULL;
	size_t i;

	for (i = 0; environ[i]; i++)
		add_node_end(&node, environ[i], 0);
	shell_info->env = node;
	return (0);
}
