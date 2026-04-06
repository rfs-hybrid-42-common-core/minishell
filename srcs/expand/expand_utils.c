/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/17 14:50:52 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:42:35 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn bool toggle_quotes(char c, bool *sq, bool *dq, int *index)
 * @brief Toggles quote state flags based on the current character.
 * @details 
 * - If single quote found and not in double quotes: toggle sq.
 * - If double quote found and not in single quotes: toggle dq.
 * - Advances index if a quote was processed.
 * @param c     The character to check.
 * @param sq    Pointer to single quote boolean flag.
 * @param dq    Pointer to double quote boolean flag.
 * @param index Pointer to the string index (incremented on match).
 * @return      true if a quote state changed, false otherwise.
 */
bool	toggle_quotes(char c, bool *sq, bool *dq, int *index)
{
	if (c == '\'' && !*dq)
	{
		*sq = !*sq;
		(*index)++;
		return (true);
	}
	if (c == '\"' && !*sq)
	{
		*dq = !*dq;
		(*index)++;
		return (true);
	}
	return (false);
}

/**
 * @fn bool remove_empty_token(t_mini *mini, t_token **curr, t_token *prev, 
 * t_token *next)
 * @brief Removes a token if its value became empty during expansion.
 * @details Specifically handles cases like `export VAR=""` where `$VAR` 
 * expands to nothing. If a token is removed, updates the linked list pointers.
 * @param mini Pointer to the main shell structure.
 * @param curr Pointer to the address of the current token.
 * @param prev Pointer to the previous token.
 * @param next Pointer to the next token.
 * @return     true if the token was removed, false otherwise.
 */
bool	remove_empty_token(t_mini *mini, t_token **curr, t_token *prev,
	t_token *next)
{
	if ((*curr)->val && *(*curr)->val == '\0')
	{
		if (prev)
			prev->next = next;
		else
			mini->tokens = next;
		free((*curr)->val);
		free(*curr);
		*curr = next;
		return (true);
	}
	return (false);
}

/**
 * @fn bool remove_backlash(char c, bool sq, bool dq)
 * @brief Determines if a backslash should be removed or kept.
 * @details 
 * - Single Quotes: Backslash is preserved.
 * - Double Quotes: Backslash is removed ONLY if followed by $, ", \, or 
 * newline.
 * - No Quotes: Backslash is always removed (it just escapes the next char).
 * @param c  The character following the backslash.
 * @param sq Boolean flag for single quotes status.
 * @param dq Boolean flag for double quotes status.
 * @return   true if the backslash should be removed, false to keep it.
 */
bool	remove_backlash(char c, bool sq, bool dq)
{
	if (sq)
		return (false);
	if (!dq)
		return (true);
	return (c == '$' || c == '\"' || c == '\\' || c == '\n');
}
