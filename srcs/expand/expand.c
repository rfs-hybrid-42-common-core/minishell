/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 18:41:16 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:45:37 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn static bool process_std_expansions(t_mini *mini, t_token **curr, 
 * t_token *prev, t_token **next)
 * @brief Helper to execute standard expansions (Tilde and Variable).
 * @details 
 * 1. Tilde expansion runs first.
 * 2. Variable expansion runs second. Checks for "ambiguous redirect" errors.
 * 3. Updates `next` because variable expansion might split words and insert 
 * new tokens.
 * @param mini Pointer to the main shell structure.
 * @param curr Double pointer to the current token.
 * @param prev Pointer to the previous token (for redirection context).
 * @param next Double pointer to the next token iterator.
 * @return     true on success, false if a critical expansion error occurred.
 */
static bool	process_std_expansions(t_mini *mini, t_token **curr, t_token *prev,
	t_token **next)
{
	expand_tilde(mini, *curr);
	if (!expand_variable(mini, curr, prev, *next))
	{
		mini->exit_code = EXIT_FAILURE;
		free_tokens(&mini->tokens);
		return (false);
	}
	*next = (*curr)->next;
	return (true);
}

/**
 * @fn static void handle_quotes_backslash(t_token *token)
 * @brief Performs final quote removal and backslash interpretation.
 * @details Rebuilds the token string character by character:
 * - Toggles quote flags but skips copying the quote characters themselves.
 * - Handles backslash escape logic via `remove_backlash`.
 * - Copies regular characters.
 * @param token The token to process.
 */
static void	handle_quotes_backslash(t_token *token)
{
	char	*new;
	bool	sq;
	bool	dq;
	int		i;

	new = ft_strdup("");
	sq = false;
	dq = false;
	i = 0;
	while (token->val[i])
	{
		if (toggle_quotes(token->val[i], &sq, &dq, &i))
			continue ;
		if (token->val[i] == '\\')
		{
			if (!remove_backlash(token->val[i + 1], sq, dq))
				new = append_char(new, token->val[i]);
			if (!token->val[++i])
				break ;
		}
		new = append_char(new, token->val[i++]);
	}
	free(token->val);
	token->val = new;
}

/**
 * @fn static bool handle_heredoc_delimiter(t_token **curr, t_token **prev, 
 * t_token *next)
 * @brief Special handling for Heredoc Delimiters.
 * @details If the previous token was `<<`, the current token is a delimiter. 
 * We must NOT expand variables (to prevent $EOF becoming empty). We only 
 * perform quote removal.
 * @param curr Pointer to current token.
 * @param prev Pointer to previous token.
 * @param next Pointer to next token.
 * @return     true if handled, false otherwise.
 */
static bool	handle_heredoc_delimiter(t_token **curr, t_token **prev,
	t_token *next)
{
	if (*prev && ((*prev)->type == TOKEN_HEREDOC
			|| (*prev)->type == TOKEN_HEREDOC_QUOTED))
	{
		if (*curr && (*curr)->val)
			handle_quotes_backslash(*curr);
		*prev = *curr;
		*curr = next;
		return (true);
	}
	return (false);
}

/**
 * @fn static bool handle_wildcard_logic(t_token **tokens, t_token **curr, 
 * t_token **prev, bool *error)
 * @brief Wraps the wildcard expansion logic.
 * @details 
 * 1. **Heredoc Check:** If the previous token indicates a Heredoc (`<<`), the 
 * current token is a delimiter. Returns false immediately to skip expansion 
 * (ensuring `<< *` uses `*` literally as the delimiter).
 * 2. Calls `expand_wildcard` to perform pattern matching.
 * 3. Checks if `expand_wildcard` set the `error` flag (Ambiguous Redirect).
 * 4. If expansion succeeded, updates `curr` and `prev` pointers to the end 
 * of the newly inserted token list.
 * @param tokens Double pointer to the head of the token list.
 * @param curr   Double pointer to the current token.
 * @param prev   Double pointer to the previous token.
 * @param error  Pointer to the error flag.
 * @return       true if expansion occurred, false otherwise (skipped or error).
 */
static bool	handle_wildcard_logic(t_token **tokens, t_token **curr,
	t_token **prev, bool *error)
{
	t_token	*wc_end;

	if (*prev && ((*prev)->type == TOKEN_HEREDOC
			|| (*prev)->type == TOKEN_HEREDOC_QUOTED))
		return (false);
	wc_end = expand_wildcard(tokens, *curr, prev, error);
	if (*error)
		return (false);
	if (wc_end)
	{
		*prev = wc_end;
		*curr = wc_end->next;
		return (true);
	}
	return (false);
}

/**
 * @fn void expand(t_mini *mini)
 * @brief Main entry point for the expansion phase.
 * @details Iterates through the token list and applies expansions in order:
 * 1. Checks for Heredoc Delimiters (skips expansion).
 * 2. **Tilde and Variable Expansion** (includes Ambiguous Redirect check).
 * 3. Empty Token Removal.
 * 4. Wildcard Expansion (*.c).
 * 5. Quote Removal & Backslash handling.
 * @param mini Pointer to the main shell structure.
 */
void	expand(t_mini *mini)
{
	t_token	*curr;
	t_token	*prev;
	t_token	*next;
	bool	error;

	error = false;
	prev = NULL;
	curr = mini->tokens;
	while (curr)
	{
		next = curr->next;
		if (handle_heredoc_delimiter(&curr, &prev, next))
			continue ;
		if (!process_std_expansions(mini, &curr, prev, &next))
			return ;
		if (remove_empty_token(mini, &curr, prev, next)
			|| handle_wildcard_logic(&mini->tokens, &curr, &prev, &error))
			continue ;
		if (error)
			return (mini->exit_code = EXIT_FAILURE, free_tokens(&mini->tokens));
		if (curr && curr->val)
			handle_quotes_backslash(curr);
		prev = curr;
		curr = next;
	}
}
