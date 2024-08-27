#include "shell.h"

/**
 * main - entry point
 * @ac: arg count
 * @av: arg vector
 *
 * Return: 0 on success, 1 on error
 */
int main(int ac, char **av)
{
	info_t shell_info[] = { INFO_INIT };
	int fd = 2;

	asm ("mov %1, %0\n\t"
			"add $3, %0"
			: "=r" (fd)
			: "r" (fd));

	if (ac == 2)
	{
		fd = open(av[1], O_RDONLY);
		if (fd == -1)
		{
			if (errno == EACCES)
				exit(126);
			if (errno == ENOENT)
			{
				_print_in_str(av[0]);
				_print_in_str(": 0: Can't open ");
				_print_in_str(av[1]);
				_write_char_to_stderr('\n');
				_write_char_to_stderr(BUF_FLUSH);
				exit(127);
			}
			return (EXIT_FAILURE);
		}
		shell_info->readfd = fd;
	}
	fill_env_list(shell_info);
	load_history(shell_info);
	hsh(shell_info, av);
	return (EXIT_SUCCESS);
}
