#include "shell.h"

/**
 * get_environ - returns the string array copy of our environ
 * @shell_info: Structure containing potential arguments. Used to maintain
 *          constant function prototype.
 * Return: Always 0
 */
char **get_environ(info_t *shell_info)
{
	if (shell_info->env_changed)
	{
		free(shell_info->environ);
		shell_info->environ = convert_list_to_strings(shell_info->env);
		shell_info->env_changed = 0;
	}

	return (shell_info->environ);
}

/**
 * _unsetenv - Remove an environment variable
 * @shell_info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 *  Return: 1 on delete, 0 otherwise
 * @var: the string env var property
 */
int _unsetenv(info_t *shell_info, const char *var)
{
	list_t *node = shell_info->env, *prev = NULL;
	char *p;

	if (!node || !var)
		return (0);

	while (node)
	{
		p = starts_with(node->str, var);
		if (p && *p == '=')
		{
			if (prev)
				prev->next = node->next;
			else
				shell_info->env = node->next;

			free(node->str);
			free(node);
			shell_info->env_changed = 1;
			return (1);
		}
		prev = node;
		node = node->next;
	}
	return (0);
}

/**
 * _setenv - Initialize a new environment variable,
 *             or modify an existing one
 * @shell_info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 * @var: the string env var property
 * @value: the string env var value
 *  Return: Always 0
 */
int _setenv(info_t *shell_info, const char *var, const char *value)
{
	char *buf;
	list_t *node;
	char *p;

	if (!var || !value)
		return (0);

	buf = malloc(strlen(var) + strlen(value) + 2);
	if (!buf)
		return (1);

	strcpy(buf, var);
	strcat(buf, "=");
	strcat(buf, value);

	for (node = shell_info->env; node; node = node->next)
	{
		p = starts_with(node->str, var);
		if (p && *p == '=')
		{
			free(node->str);
			node->str = buf;
			shell_info->env_changed = 1;
			return (0);
		}
	}

	add_node_end(&(shell_info->env), buf, 0);
	shell_info->env_changed = 1;
	return (0);
}
