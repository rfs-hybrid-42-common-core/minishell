/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:21:46 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 15:46:19 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "libft.h"
# include "parser.h"
# include "token.h"
# include "utils.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <unistd.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def PID_SIZE
 * @brief Maximum buffer size for the PID string.
 * @details Used to allocate memory when caching the shell's PID as a string 
 * for `$$` expansion.
 */
# ifndef PID_SIZE
#  define PID_SIZE 32
# endif

/**
 * @def EXIT_SYNTAX_ERROR
 * @brief Exit status for syntax errors.
 * @details Standard Bash exit code (2) returned when a command line fails 
 * parsing due to misplaced operators or unclosed quotes.
 */
# define EXIT_SYNTAX_ERROR	2

/**
 * @def EXIT_NO_EXEC
 * @brief Exit status for permission denied or directory execution attempts.
 * @details Standard Bash exit code (126) returned when a command is found 
 * but lacks execution permissions, or when the user attempts to execute a 
 * directory directly.
 */
# define EXIT_NO_EXEC		126

/**
 * @def EXIT_NOT_FOUND
 * @brief Exit status for missing commands or files.
 * @details Standard Bash exit code (127) returned when a command does not 
 * exist in the PATH, or an absolute/relative path points to a non-existent 
 * file.
 */
# define EXIT_NOT_FOUND		127

/**
 * @def EXIT_SIGINT
 * @brief Exit status for a process terminated by SIGINT.
 * @details Standard POSIX exit code (130) for a process killed by `Ctrl+C`. 
 * Calculated as 128 + 2 (SIGINT).
 */
# define EXIT_SIGINT		130

/**
 * @def EXIT_SIGQUIT
 * @brief Exit status for a process terminated by SIGQUIT.
 * @details Standard POSIX exit code (131) for a process killed by `Ctrl+\`. 
 * Calculated as 128 + 3 (SIGQUIT).
 */
# define EXIT_SIGQUIT		131

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_env
 * @brief Forward declaration of the environment variable structure.
 * @details Represents a node in the shell's environment linked list. 
 * It holds the variable's key, its value, and a boolean flag indicating 
 * whether the variable should be exported to child processes or kept as 
 * a local shell variable.
 */
typedef struct s_env	t_env;

/**
 * @typedef t_ast
 * @brief Forward declaration of the Abstract Syntax Tree node.
 * @details Acts as the core building block for command execution order. 
 * It represents either an operator (like pipes or logical AND/OR) bridging 
 * two child nodes, or a simple command/subshell ready to be executed.
 */
typedef struct s_ast	t_ast;

/**
 * @struct s_prompt
 * @brief Handles prompt display information.
 * @details Encapsulates the state needed to render the interactive 
 * prompt and store the user's raw input before tokenization.
 * @var login The formatted string "user@host:dir$ ".
 * @var line  The actual input line read from readline or GNL.
 */
typedef struct s_prompt
{
	char	*login;
	char	*line;
}	t_prompt;

/**
 * @struct s_io
 * @brief Stores original file descriptors for input/output restoration.
 * @details Used primarily during the execution of builtins within the 
 * main process to ensure the shell's standard I/O is preserved and 
 * restored after redirections are applied.
 * @var in  Copy of the original STDIN_FILENO.
 * @var out Copy of the original STDOUT_FILENO.
 */
typedef struct s_io
{
	int	in;
	int	out;
}	t_io;

/**
 * @struct s_mini
 * @brief Main shell control structure.
 * @details Holds all the state required for the shell to function, passed
 * down to almost every function.
 * @var prompt     Prompt display and input handling.
 * @var io         IO backup for redirection restoration.
 * @var env        Linked list of environment variables.
 * @var tokens     Linked list of tokens derived from input.
 * @var ast        Root of the Abstract Syntax Tree.
 * @var exit_code  The exit status of the last executed command.
 * @var line_count The current line number (used for heredoc warnings and error 
 * messages).
 * @var pid_str    String representation of the shell's PID, cached for 
 * $$ expansion.
 */
typedef struct s_mini
{
	t_prompt	prompt;
	t_io		io;
	t_env		*env;
	t_token		*tokens;
	t_ast		*ast;
	int			exit_code;
	int			line_count;
	char		*pid_str;
}	t_mini;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Initialization ----------------------------- */
void	init_minishell(t_mini *mini, char **envp);

/* --------------------------------- Prompt --------------------------------- */
void	get_prompt_info(t_prompt *prompt, t_env *env);

/* -------------------------------- Heredoc --------------------------------- */
bool	process_heredoc(t_mini *mini, t_ast *ast);

/* -------------------------------- Cleanup --------------------------------- */
void	cleanup_iteration(t_mini *mini);
void	cleanup_heredoc(t_ast *ast);

#endif
