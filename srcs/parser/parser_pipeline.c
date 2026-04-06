/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 14:52:22 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 04:49:49 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @fn static t_ast *fetch_pipeline_right(t_token **tokens)
 * @brief Parses the right-hand side of a pipe and handles syntax errors.
 * @details 
 * 1. Saves the current token position.
 * 2. Calls parse_single_command().
 * 3. Error Handling:
 * - If the parser returns NULL (failure), checks if any tokens were consumed.
 * - If tokens moved (e.g., 'cat | <'), the child parser already reported 
 * the error, so we stay silent.
 * - If tokens did NOT move (e.g., 'ls |' -> EOF), we print "unexpected 
 * newline" here.
 * @param tokens Double pointer to the current token list.
 * @return       Pointer to the right AST node, or NULL on failure.
 */
static t_ast	*fetch_pipeline_right(t_token **tokens)
{
	t_ast	*right;
	t_token	*start;

	start = *tokens;
	right = parse_single_command(tokens);
	if (!right)
	{
		if (*tokens == start && (!*tokens || (*tokens)->type == TOKEN_END))
			print_syntax_error("near unexpected token", "newline");
	}
	return (right);
}

/**
 * @fn t_ast *parse_pipeline(t_token **tokens)
 * @brief Parses pipeline sequences (e.g., cmd1 | cmd2 | cmd3).
 * @details 
 * - Has higher precedence than logical operators (&&, ||).
 * - Iteratively builds a tree of AST_PIPE nodes connecting commands.
 * - Uses fetch_pipeline_right() to safely parse operands and manage 
 * error reporting logic (avoiding double error messages).
 * @param tokens Double pointer to current token list.
 * @return       Pointer to the root of the pipeline subtree.
 */
t_ast	*parse_pipeline(t_token **tokens)
{
	t_ast	*left;
	t_ast	*right;
	t_ast	*pipe;

	left = parse_single_command(tokens);
	if (!left)
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		(*tokens) = (*tokens)->next;
		right = fetch_pipeline_right(tokens);
		if (!right)
			return (free_ast(left), NULL);
		pipe = new_tree_node(AST_PIPE);
		if (!pipe)
			return (free_ast(left), free_ast(right), NULL);
		pipe->lnode = left;
		pipe->rnode = right;
		left = pipe;
	}
	return (left);
}
