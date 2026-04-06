/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 18:26:40 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 04:50:30 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @fn static char *get_token_symbol(t_token *token)
 * @brief Retrieves the string representation of a token type.
 * @details Used for error messages (e.g., "syntax error near token '|'").
 * @param token The token to describe.
 * @return      String literal representing the token.
 */
static char	*get_token_symbol(t_token *token)
{
	if (token->type == TOKEN_PIPE)
		return ("|");
	if (token->type == TOKEN_OR)
		return ("||");
	if (token->type == TOKEN_AND)
		return ("&&");
	if (token->type == TOKEN_LPAREN)
		return ("(");
	if (token->type == TOKEN_RPAREN)
		return (")");
	if (token->type == TOKEN_REDIR_IN)
		return ("<");
	if (token->type == TOKEN_REDIR_OUT)
		return (">");
	if (token->type == TOKEN_APPEND)
		return (">>");
	if (token->type == TOKEN_HEREDOC || token->type == TOKEN_HEREDOC_QUOTED)
		return ("<<");
	if (token->type == TOKEN_WORD)
		return (token->val);
	return ("newline");
}

/**
 * @fn void parser(t_mini *mini)
 * @brief Entry point for the Abstract Syntax Tree (AST) construction.
 * @details 
 * 1. Calls parse_logic to build the AST from the token list.
 * 2. Validates that all tokens were consumed (reached TOKEN_END).
 * 3. Handles syntax errors if trailing tokens remain or if parsing failed.
 * @param mini Pointer to the main shell structure.
 */
void	parser(t_mini *mini)
{
	t_token	*curr;

	if (!mini->tokens)
		return ;
	curr = mini->tokens;
	mini->ast = parse_logic(&curr);
	if (curr && curr->type != TOKEN_END)
	{
		print_syntax_error("near unexpected token", get_token_symbol(curr));
		if (mini->ast)
			free_ast(mini->ast);
		mini->ast = NULL;
		mini->exit_code = EXIT_SYNTAX_ERROR;
	}
	if (!mini->ast && mini->tokens && mini->tokens->type != TOKEN_END)
		mini->exit_code = EXIT_SYNTAX_ERROR;
}
