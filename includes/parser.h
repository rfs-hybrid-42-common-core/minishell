/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:01:33 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 15:50:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "minishell.h"
# include "token.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdio.h>

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @enum e_ast_type
 * @brief Identifies the type of an Abstract Syntax Tree node.
 * @details Used by the execution engine to determine how to handle 
 * a specific node, distinguishing between executable commands, 
 * subshells, and structural operators like pipes or logic gates.
 * @var AST_CMD      Represents a simple executable command (leaf node).
 * @var AST_PIPE     Represents a pipeline operator '|'.
 * @var AST_AND      Represents a logical AND operator '&&'.
 * @var AST_OR       Represents a logical OR operator '||'.
 * @var AST_SUBSHELL Represents a subshell block enclosed in parenthesis '()'.
 */
typedef enum e_ast_type
{
	AST_CMD,
	AST_PIPE,
	AST_AND,
	AST_OR,
	AST_SUBSHELL
}	t_ast_type;

/**
 * @struct s_redir
 * @brief Linked list node for input/output redirections.
 * @details Stores the type and target of a redirection operation 
 * (e.g., ">", "<", ">>", "<<") associated with a specific command 
 * or subshell, processed before command execution.
 * @var type The token type representing the redirection operator.
 * @var file The target filename or the heredoc delimiter string.
 * @var next Pointer to the next redirection node in the chain.
 */
typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

/**
 * @struct s_cmd
 * @brief Execution parameters for a simple command.
 * @details Encapsulates the parsed arguments and any associated 
 * file redirections for a single command, making it ready to be 
 * passed to execve or a builtin handler.
 * @var argv   Null-terminated array of command arguments.
 * @var redirs Head of the linked list of redirections for this command.
 */
typedef struct s_cmd
{
	char	**argv;
	t_redir	*redirs;
}	t_cmd;

/**
 * @struct s_ast
 * @brief Node in the Abstract Syntax Tree (Binary Tree).
 * @details Forms the hierarchical structure of the parsed command 
 * line, strictly enforcing the order of operations and precedence 
 * for pipes, logical operators, and subshells.
 * @var type  The type of node (Operator or Command).
 * @var lnode Left child (Left operand or Command).
 * @var rnode Right child (Right operand or NULL for Subshell/Command).
 * @var cmd   Data for simple command (NULL if node type is Operator).
 */
typedef struct s_ast
{
	t_ast_type		type;
	struct s_ast	*lnode;
	struct s_ast	*rnode;
	t_cmd			*cmd;
}	t_ast;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ------------------------------ Main Parser ------------------------------- */
void	parser(t_mini *mini);

/* ---------------------- Recursive Descent Functions ----------------------- */
t_ast	*parse_single_command(t_token **tokens);
t_ast	*parse_pipeline(t_token **tokens);
t_ast	*parse_logic(t_token **tokens);
t_ast	*parse_subshell(t_token **tokens);
bool	parse_subshell_redirs(t_ast *node, t_token **tokens);

/* ------------------------ AST Construction Helpers ------------------------ */
t_cmd	*new_cmd(void);
t_ast	*new_tree_node(t_ast_type type);
t_redir	*new_redir(t_token_type type, char *file);
bool	cmd_add_redir(t_cmd *cmd, t_token_type type, char *file);

#endif
