/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_logic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 16:33:52 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 04:49:33 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @fn static t_ast *fetch_logic_right(t_token **tokens)
 * @brief Parses the right-hand side of a logical operator (&& or ||).
 * @details 
 * 1. Saves the current token position.
 * 2. Calls parse_pipeline() (since logic ops wrap pipelines).
 * 3. Error Handling:
 * - If parse_pipeline returns NULL, checks if tokens were consumed.
 * - If tokens moved, assumes specific error was already printed.
 * - If tokens did NOT move (e.g., 'cmd &&' -> EOF), prints "unexpected 
 * newline" error.
 * @param tokens Double pointer to the current token list.
 * @return       Pointer to the right AST node, or NULL on failure.
 */
static t_ast	*fetch_logic_right(t_token **tokens)
{
	t_ast	*right;
	t_token	*start;

	start = *tokens;
	right = parse_pipeline(tokens);
	if (!right)
	{
		if (*tokens == start && (!*tokens || (*tokens)->type == TOKEN_END))
			print_syntax_error("near unexpected token", "newline");
	}
	return (right);
}

/**
 * @fn t_ast *parse_logic(t_token **tokens)
 * @brief Parses logical operators (&& and ||).
 * @details 
 * - Has the lowest precedence in the grammar.
 * - Iteratively builds a tree of AST_AND / AST_OR nodes.
 * - Uses fetch_logic_right() to safely parse the right operand and ensure 
 * errors are reported exactly once.
 * @param tokens Double pointer to current token list.
 * @return       Pointer to the root of the logic subtree.
 */
t_ast	*parse_logic(t_token **tokens)
{
	t_ast	*ast;
	t_ast	*left;
	t_ast	*right;

	left = parse_pipeline(tokens);
	if (!left)
		return (NULL);
	while (*tokens && ((*tokens)->type == TOKEN_AND
			|| (*tokens)->type == TOKEN_OR))
	{
		if ((*tokens)->type == TOKEN_AND)
			ast = new_tree_node(AST_AND);
		else
			ast = new_tree_node(AST_OR);
		if (!ast)
			return (free_ast(left), NULL);
		(*tokens) = (*tokens)->next;
		right = fetch_logic_right(tokens);
		if (!right)
			return (free(ast), free_ast(left), NULL);
		ast->lnode = left;
		ast->rnode = right;
		left = ast;
	}
	return (left);
}
