/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:01:12 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 16:53:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "libft.h"
# include "minishell.h"
# include "token.h"
# include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <errno.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def BUFFER_SIZE
 * @brief Default size for I/O buffers.
 * @details Used throughout the utility functions to allocate appropriately 
 * sized temporary arrays, ensuring memory safety without hardcoding magic 
 */
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 256
# endif

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_cmd
 * @brief Forward declaration of the simple command structure.
 * @details Encapsulates the execution parameters for a single command, 
 * containing the null-terminated array of arguments (argv) and the 
 * linked list of file redirections associated with it.
 */
typedef struct s_cmd	t_cmd;

/**
 * @typedef t_ast
 * @brief Forward declaration of the Abstract Syntax Tree node.
 * @details Represents a node in the binary tree used by the parser to 
 * define execution precedence. Declared here to allow utility functions 
 * to traverse and safely deallocate the tree's memory.
 */
typedef struct s_ast	t_ast;

/**
 * @typedef t_prompt
 * @brief Forward declaration of the prompt structure.
 * @details Manages the interactive prompt's display information. It 
 * stores both the formatted login string (e.g., user@host:dir$) and 
 * the actual input line read from the user.
 */
typedef struct s_prompt	t_prompt;

/**
 * @struct s_env
 * @brief Linked list node for environment variables.
 * @details Acts as the primary storage mechanism for both exported 
 * environment variables and local shell variables, allowing dynamic 
 * modification and retrieval during execution.
 * @var key      The variable name (e.g., "PATH").
 * @var val      The variable value (e.g., "/bin:/usr/bin"). Can be NULL.
 * @var exported Boolean flag: true if variable should appear in child 
 * processes (env), false if it's a local shell variable.
 * @var next     Pointer to the next node.
 */
typedef struct s_env
{
	char			*key;
	char			*val;
	bool			exported;
	struct s_env	*next;
}	t_env;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ------------------------- Environment List Utils ------------------------- */
t_env	*find_env(t_env *env, char *key);
int		get_env_size(t_env *env);
t_env	*env_new(char *key, char *val, bool exported);
void	env_add_back(t_env **env, t_env *new);
void	env_list_clear(t_env **env);

/* ---------------------- Environment Array Conversion ---------------------- */
void	sort_env_array(char **envp, int size);
char	**env_list_to_array(t_env *env);

/* ----------------------------- Error Handling ----------------------------- */
void	print_command_error(char *cmd, char *msg);
void	print_export_error(char *arg, char *msg);
void	print_syntax_error(char *msg, char *token);
void	print_write_error(char *msg);
void	print_end_error(t_token *token);

/* -------------------------- Memory Deallocation --------------------------- */
void	free_cmd(t_cmd *cmd);
void	free_ast(t_ast *ast);
void	free_tokens(t_token **tokens);
void	free_matrix(char **arr);
void	free_mini(t_mini *mini);

#endif
