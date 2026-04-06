/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 04:09:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:30:06 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn static int get_word_len(char *str)
 * @brief Calculates the length of the next word segment for variable splitting.
 * @details Scans the string until a non-escaped, non-quoted space is found.
 * - **Quote Handling**: Ignores spaces enclosed in single or double quotes.
 * - **Escape Handling**: If a backslash is encountered (outside single 
 * quotes), it skips the backslash and the next character. This ensures that 
 * escaped quotes (like `\"`) are treated as literal characters and do not 
 * toggle quote mode, allowing correct splitting of expanded variables.
 * @param str The string to scan.
 * @return    The length of the word.
 */
static int	get_word_len(char *str)
{
	bool	sq;
	bool	dq;
	int		len;

	len = 0;
	sq = false;
	dq = false;
	while (str[len])
	{
		if (!sq && str[len] == '\\' && str[len + 1])
		{
			len += 2;
			continue ;
		}
		if (toggle_quotes(str[len], &sq, &dq, &len))
			continue ;
		if (!sq && !dq && ft_isspace(str[len]))
			break ;
		len++;
	}
	return (len);
}

/**
 * @fn static void handle_first_word(t_token **curr, char *str, int *i)
 * @brief Updates the current token with the first word of the split.
 * @details Replaces the value of `curr` with the substring of the first word. 
 * If the string contains only spaces, `curr` becomes an empty string (to be 
 * removed later).
 * @param curr Double pointer to the current token.
 * @param str  The expanded string containing multiple words.
 * @param i    Pointer to the index in `str` (updated to end of first word).
 */
static void	handle_first_word(t_token **curr, char *str, int *i)
{
	int	len;

	while (str[*i] && ft_isspace(str[*i]))
		(*i)++;
	if (!str[*i])
	{
		free((*curr)->val);
		(*curr)->val = ft_strdup("");
		return ;
	}
	len = get_word_len(str + *i);
	free((*curr)->val);
	(*curr)->val = ft_substr(str, *i, len);
	*i += len;
}

/**
 * @fn static void append_next_word(t_token **token, char *str, int *i)
 * @brief Creates new tokens for subsequent words.
 * @details Allocates a new TOKEN_WORD node for the next word found in `str` 
 * and links it into the list after the current token.
 * @param token Double pointer to the current token (moves to the new node).
 * @param str   The expanded string.
 * @param i     Pointer to the index in `str` (updated to end of word).
 */
static void	append_next_word(t_token **token, char *str, int *i)
{
	int		len;
	char	*val;
	t_token	*new;

	while (str[*i] && ft_isspace(str[*i]))
		(*i)++;
	if (!str[*i])
		return ;
	len = get_word_len(str + *i);
	val = ft_substr(str, *i, len);
	new = token_new(TOKEN_WORD, val);
	if (new)
	{
		new->next = (*token)->next;
		(*token)->next = new;
		*token = new;
	}
	*i += len;
}

/**
 * @fn void expand_split(t_token **curr, char *str)
 * @brief Splits an expanded variable string into multiple tokens.
 * @details Called when an unquoted variable expands to a string containing 
 * spaces (e.g., export A="ls -l"; $A).
 * 1. Updates the current token with the first word.
 * 2. Creates new tokens for remaining words and inserts them into the list.
 * @param curr Double pointer to the token being expanded.
 * @param str  The result of the variable expansion.
 */
void	expand_split(t_token **curr, char *str)
{
	t_token	*token;
	int		i;

	i = 0;
	handle_first_word(curr, str, &i);
	token = *curr;
	while (str[i])
		append_next_word(&token, str, &i);
}
