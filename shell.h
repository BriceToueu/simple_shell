#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

/* for read/write buffers */
#define READ_BUF_SIZE 1024
#define WRITE_BUF_SIZE 1024
#define BUF_FLUSH -1

/* for command chaining */
#define CMD_NORM	0
#define CMD_OR		1
#define CMD_AND		2
#define CMD_CHAIN	3

/* for convert_number() */
#define CONVERT_LOWERCASE	1
#define CONVERT_UNSIGNED	2

/* 1 if using system getline() */
#define USE_GETLINE 0
#define USE_STRTOK 0

#define HIST_FILE	".simple_shell_history"
#define HIST_MAX	4096

extern char **environ;

/**
 * struct liststr - singly linked list
 * @num: the number field
 * @str: a string
 * @next: points to the next node
 */
typedef struct liststr
{
	int num;
	char *str;
	struct liststr *next;
} list_t;

/**
 * struct passinfo - contains pseudo-arguements to pass into a function,
 * allowing uniform prototype for function pointer struct
 * @arg: a string generated from getline containing arguements
 * @argv:an array of strings generated from arg
 * @path: a string path for the current command
 * @argc: the argument count
 * @line_count: the error count
 * @err_num: the error code for exit()s
 * @linecount_flag: if on count this line of input
 * @fname: the program filename
 * @env: linked list local copy of environ
 * @environ: custom modified copy of environ from LL env
 * @history: the history node
 * @alias: the alias node
 * @env_changed: on if environ was changed
 * @status: the return status of the last exec'd command
 * @cmd_buf: address of pointer to cmd_buf, on if chaining
 * @cmd_buf_type: CMD_type ||, &&, ;
 * @readfd: the fd from which to read line input
 * @histcount: the history line number count
 */
typedef struct passinfo
{
	char *arg;
	char **argv;
	char *path;
	int argc;
	unsigned int line_count;
	int err_num;
	int linecount_flag;
	char *fname;
	list_t *env;
	list_t *history;
	list_t *alias;
	char **environ;
	int env_changed;
	int status;

	char **cmd_buf; /* pointer to cmd ; chain buffer, for memory mangement */
	int cmd_buf_type; /* CMD_type ||, &&, ; */
	int readfd;
	int histcount;
} info_t;

#define INFO_INIT \
{NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, \
		0, 0, 0}

/**
 * struct builtin - contains a builtin string and related function
 * @type: the builtin command flag
 * @func: the function
 */
typedef struct builtin
{
	char *type;
	int (*func)(info_t *);
} builtin_table;

/* shell_loop.c*/
int hsh(info_t *shell_info, char **av);
int find_builtin(info_t *shell_info);
void find_executable(info_t *shell_info);
void fork_executable(info_t *shell_info);

/* command_parser.c */
int is_executable(info_t *shell_info, char *path);
char *copy_chars(char *pathstr, int start, int stop);
char *find_path(info_t *shell_info, char *pathstr, char *cmd);

/* string_utils.c */
void _print_in_str(char *str);
int _write_char_to_stderr(char c);
int _write_char_to_fd(char c, int fd);
int print_string_to_fd(char *str, int fd);

/* string_file.c */
int _strlen(char *s);
int _strcmp(char *s1, char *s2);
char *starts_with(const char *haystack, const char *needle);
char *_strcat(char *dest, char *src);

/* string_file1.c */
char *_strcpy(char *dest, char *src);
char *_strdup(const char *str);
void _puts(char *str);
int _putchar(char c);

/* string_operations.c */
char *_strncpy(char *dest, char *src, int n);
char *_strncat(char *dest, char *src, int n);
char *_strchr(char *s, char c);

/* tokenizer */
char **split_string(char *str, char *d);
char **split_string_single_delim(char *str, char d);

/* memory_management.c */
char *_memset(char *s, char b, unsigned int n);
void ffree(char **pp);
void *_realloc(void *ptr, unsigned int old_size, unsigned int new_size);

/* memory_utils.c */
int free_pointer(void **ptr);

/* helpers.c */
int is_interactive_mode(info_t *shell_info);
int is_character_delim(char c, char *del);
int is_alphabetic(int c);
int string_to_int(char *s);

/* utility_functions.c */
int str_to_int(char *s);
void print_error(info_t *info, char *estr);
int print_decimal(int input, int fd);
char *number_to_string(long int num, int base, int flags);
void strip_comments(char *buf);

/* shell_operations.c */
int exit_shell(info_t *shell_info);
int change_directory(info_t *shell_info);
int display_help(info_t *shell_info);

/* alias.c */
int display_history(info_t *shell_info);
int remove_alias(info_t *shell_info, char *str);
int assign_alias(info_t *shell_info, char *str);
int display_alias(list_t *node);
int mimic_alias(info_t *shell_info);

/* getLine.c */
ssize_t input_buffer(info_t *shell_info, char **buf, size_t *len);
ssize_t get_input(info_t *shell_info);
ssize_t read_buffer(info_t *shell_info, char *buf, size_t *i);
int _getline(info_t *shell_info, char **ptr, size_t *length);
void sigintHandler(__attribute__((unused))int sig_num);

/* info_management.c */
void initialize_info(info_t *shell_info);
void initialize_struct(info_t *shell_info, char **av);
void release_info(info_t *shell_info, int all);

/* env_functions.c */
int print_environment(info_t *shell_info);
char *get_env(info_t *shell_info, const char *name);
int set_environment_variable(info_t *shell_info);
int unset_environment_variable(info_t *shell_info);
int fill_env_list(info_t *shell_info);

/* getEnv.c */
char **get_environ(info_t *shell_info);
int _unsetenv(info_t *shell_info, const char *var);
int _setenv(info_t *shell_info, const char *var, const char *value);

/* history_management */
char *fetch_history_file(info_t *shell_info);
int save_history(info_t *shell_info);
int load_history(info_t *shell_info);
int append_history_list(info_t *shell_info, char *buffer, int linecount);
int update_history_numbers(info_t *shell_info);

/* list_operations.c */
list_t *add_node(list_t **head, const char *str, int num);
list_t *add_node_end(list_t **head, const char *str, int num);
size_t print_list_str(const list_t *h);
int delete_node_at_index(list_t **head, unsigned int index);
void free_list(list_t **head_ptr);

/* additional_linked_list_operations.c */
size_t get_list_length(const list_t *h);
char **convert_list_to_strings(list_t *head);
size_t display_list(const list_t *h);
list_t *find_node_with_prefix(list_t *node, char *prefix, char c);
ssize_t get_node_position(list_t *head, list_t *node);

/* variable_replacement.c */
int is_chain(info_t *shell_info, char *buf, size_t *p);
void check_chain(info_t *shell_info, char *buf, size_t *p, size_t i, size_t len);
int replace_alias(info_t *shell_info);
int replace_vars(info_t *shell_info);
int replace_string(char **old, char *new);

#endif
