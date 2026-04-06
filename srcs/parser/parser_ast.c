/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_ast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 20:02:59 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 16:11:27 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @fn t_cmd *new_cmd(void)
 * @brief Allocates and initializes a new command structure.
 * @details Allocates memory for a t_cmd node and sets its internal pointers 
 * (argv and redirs) to NULL to prevent undefined behavior during execution 
 * or cleanup.
 * @return Pointer to the new t_cmd, or NULL on failure.
 */
t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	return (cmd);
}

/**
 * @fn t_redir *new_redir(t_token_type type, char *file)
 * @brief Allocates and initializes a new redirection node.
 * @details Duplicates the provided file/delimiter string and assigns the 
 * specific redirection type. Used to build the linked list of redirections 
 * attached to a command or subshell.
 * @param type The type of redirection (TOKEN_REDIR_IN, etc.).
 * @param file The filename or delimiter string (duplicated).
 * @return     Pointer to the new t_redir, or NULL on failure.
 */
t_redir	*new_redir(t_token_type type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->file = ft_strdup(file);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->type = type;
	redir->next = NULL;
	return (redir);
}

/**
 * @fn t_ast *new_tree_node(t_ast_type type)
 * @brief Allocates and initializes a new AST node.
 * @details Creates a generic AST node with the given type (Command, Pipe, 
 * Logic, Subshell). All child pointers and command data are explicitly 
 * initialized to NULL.
 * @param type The architectural type of the node.
 * @return     Pointer to the new t_ast node, or NULL on failure.
 */
t_ast	*new_tree_node(t_ast_type type)
{
	t_ast	*new_node;

	new_node = malloc(sizeof(t_ast));
	if (!new_node)
		return (NULL);
	new_node->type = type;
	new_node->cmd = NULL;
	new_node->lnode = NULL;
	new_node->rnode = NULL;
	return (new_node);
}
