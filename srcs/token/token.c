/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:39:44 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 16:13:33 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token.h"
#include "utils.h"

/**
 * @fn static bool multi_char_op(t_token **tokens, char *input, int *index)
 * @brief checks for and handles two-character operators.
 * @details Detects '>>', '>|', '<<', '&&', '||'. Creates the corresponding 
 * token and advances the index by 2.
 * @param tokens Pointer to the token list head.
 * @param input  The input string.
 * @param index  Pointer to current index.
 * @return       true if a multi-char operator was found, false otherwise.
 */
static bool	multi_char_op(t_token **tokens, char *input, int *index)
{
	t_token_type	type;

	if (input[*index] == '>' && input[*index + 1] == '>')
		type = TOKEN_APPEND;
	else if (input[*index] == '>' && input[*index + 1] == '|')
		type = TOKEN_REDIR_OUT;
	else if (input[*index] == '<' && input[*index + 1] == '<')
		type = TOKEN_HEREDOC;
	else if (input[*index] == '&' && input[*index + 1] == '&')
		type = TOKEN_AND;
	else if (input[*index] == '|' && input[*index + 1] == '|')
		type = TOKEN_OR;
	else
		return (false);
	token_add_back(tokens, token_new(type, NULL));
	*index += 2;
	return (true);
}

/**
 * @fn static bool single_char_op(t_token **tokens, char *input, int *index)
 * @brief Checks for and handles single-character operators.
 * @details Detects '|', '<', '>', '(', ')'. Creates the corresponding 
 * token and advances the index by 1.
 * @param tokens Pointer to the token list head.
 * @param input  The input string.
 * @param index  Pointer to current index.
 * @return       true if a single-char operator was found, false otherwise.
 */
static bool	single_char_op(t_token **tokens, char *input, int *index)
{
	t_token_type	type;

	if (input[*index] == '|')
		type = TOKEN_PIPE;
	else if (input[*index] == '<')
		type = TOKEN_REDIR_IN;
	else if (input[*index] == '>')
		type = TOKEN_REDIR_OUT;
	else if (input[*index] == '(')
		type = TOKEN_LPAREN;
	else if (input[*index] == ')')
		type = TOKEN_RPAREN;
	else
		return (false);
	token_add_back(tokens, token_new(type, NULL));
	(*index)++;
	return (true);
}

/**
 * @fn static int handle_word(t_token **tokens, char *input, int *index)
 * @brief Handles regular words (commands, arguments, filenames).
 * @details Calls collect_word to extract the string and adds a TOKEN_WORD node.
 * @param tokens Pointer to the token list head.
 * @param input  The input string.
 * @param index  Pointer to current index.
 * @return       1 on success, 0 if it's an operator, -1 on allocation failure.
 */
static int	handle_word(t_token **tokens, char *input, int *index)
{
	t_token	*new;

	if (is_op(input[*index]))
		return (0);
	new = token_new(TOKEN_WORD, NULL);
	if (!new)
		return (-1);
	new->val = collect_word(input, index);
	if (!new->val)
	{
		free(new);
		return (-1);
	}
	token_add_back(tokens, new);
	return (1);
}

/**
 * @fn static int handle_token(t_token **tokens, char *input, int *index)
 * @brief Dispatcher function for token recognition.
 * @details Tries to match multi-char ops, then single-char ops, then words.
 * Consolidates the parsing logic into a single routing function.
 * @param tokens Double pointer to the token list head.
 * @param input  The raw input string from the prompt.
 * @param index  Pointer to the current index in the input string.
 * @return       1 on success, -1 on fatal error.
 */
static int	handle_token(t_token **tokens, char *input, int *index)
{
	if (multi_char_op(tokens, input, index))
		return (1);
	if (single_char_op(tokens, input, index))
		return (1);
	return (handle_word(tokens, input, index));
}

/**
 * @fn bool tokenizer(t_mini *mini)
 * @brief Main tokenization loop.
 * @details Scans the input line, skipping spaces and identifying tokens. 
 * Handles errors like unclosed quotes (status -1) by printing syntax errors. 
 * Appends a TOKEN_END at the list tail.
 * @param mini Pointer to the main shell structure.
 * @return     true on success, false on syntax/memory error.
 */
bool	tokenizer(t_mini *mini)
{
	char	str[2];
	int		status;
	int		i;

	i = 0;
	while (mini->prompt.line[i])
	{
		while (ft_isspace(mini->prompt.line[i]))
			i++;
		if (!mini->prompt.line[i])
			break ;
		status = handle_token(&mini->tokens, mini->prompt.line, &i);
		if (status > 0)
			continue ;
		if (status == -1)
			print_syntax_error("unclosed quotes or memory failure", NULL);
		else
		{
			ft_strlcpy(str, mini->prompt.line + i, 2);
			print_syntax_error("near unexpected token", str);
		}
		mini->exit_code = EXIT_SYNTAX_ERROR;
		return (false);
	}
	return (token_add_back(&mini->tokens, token_new(TOKEN_END, NULL)), true);
}
