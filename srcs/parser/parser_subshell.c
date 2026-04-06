/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_subshell.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:43:32 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 04:50:22 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @fn bool parse_subshell_redirs(t_ast *node, t_token **tokens)
 * @brief Parses redirections attached directly to a subshell.
 * @details 
 * - Called after ')' is consumed.
 * - Checks for redirection tokens (<, >, >>, <<).
 * - Allocates the node->cmd structure if needed and appends redirections 
 * to it, allowing the executor to handle "(cmd) > out" correctly.
 * @param node   The AST_SUBSHELL node to attach redirections to.
 * @param tokens Double pointer to the current token list.
 * @return       true on success, false on parsing/allocation error.
 */
bool	parse_subshell_redirs(t_ast *node, t_token **tokens)
{
	t_token_type	type;

	while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN
			|| (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_APPEND
			|| (*tokens)->type == TOKEN_HEREDOC))
	{
		if (!node->cmd)
			node->cmd = new_cmd();
		if (!node->cmd)
			return (false);
		type = (*tokens)->type;
		*tokens = (*tokens)->next;
		if (!*tokens || (*tokens)->type != TOKEN_WORD)
			return (print_end_error(*tokens), false);
		if (!cmd_add_redir(node->cmd, type, (*tokens)->val))
			return (false);
		*tokens = (*tokens)->next;
	}
	return (true);
}

/**
 * @fn t_ast *parse_subshell(t_token **tokens)
 * @brief Parses a subshell block surrounded by parenthesis ( ... ).
 * @details 
 * 1. Consumes '(' [TOKEN_LPAREN].
 * 2. Recursively calls parse_logic to parse the inner content.
 * 3. Consumes ')' [TOKEN_RPAREN].
 * 4. Wraps result in an AST_SUBSHELL node.
 * @param tokens Double pointer to current token list.
 * @return       Pointer to the subshell node, or NULL on error.
 */
t_ast	*parse_subshell(t_token **tokens)
{
	t_ast	*node;
	t_ast	*inner;

	if (!*tokens || (*tokens)->type != TOKEN_LPAREN)
		return (NULL);
	*tokens = (*tokens)->next;
	inner = parse_logic(tokens);
	if (!inner)
		return (NULL);
	if (!*tokens || (*tokens)->type != TOKEN_RPAREN)
	{
		print_syntax_error("expected `)'", NULL);
		return (free_ast(inner), NULL);
	}
	*tokens = (*tokens)->next;
	node = new_tree_node(AST_SUBSHELL);
	if (!node)
		return (free_ast(inner), NULL);
	node->lnode = inner;
	node->rnode = NULL;
	node->cmd = NULL;
	return (node);
}
