/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 15:26:35 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:32:46 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn bool has_wildcard(t_token *token)
 * @brief Checks if a token contains an unquoted wildcard (*).
 * @details Iterates through the token value respecting quotes. 
 * Only returns true if a '*' exists outside of quotes.
 * @param token The token to check.
 * @return      true if expansion is needed, false otherwise.
 */
bool	has_wildcard(t_token *token)
{
	bool	sq;
	bool	dq;
	int		i;

	if (!token || token->type != TOKEN_WORD || !token->val)
		return (false);
	sq = false;
	dq = false;
	i = -1;
	while (token->val[++i])
	{
		if (token->val[i] == '\'' && !dq)
			sq = !sq;
		else if (token->val[i] == '\"' && !sq)
			dq = !dq;
		else if (token->val[i] == '*' && !sq && !dq)
			return (true);
	}
	return (false);
}

/**
 * @fn char *get_wildcard_str(char *str)
 * @brief Extracts the raw pattern string for matching.
 * @details Removes quotes but preserves the wildcard characters. 
 * Marks active wildcards with a special WILDCARD sentinel if needed, 
 * allowing the recursion logic to distinguish literal '*' from glob '*'.
 * @param str The original token string.
 * @return    Allocated string representing the clean pattern.
 */
char	*get_wildcard_str(char *str)
{
	char	*new;
	bool	sq;
	bool	dq;
	int		i;
	int		j;

	new = malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!new)
		return (NULL);
	sq = false;
	dq = false;
	i = 0;
	j = 0;
	while (str[i])
	{
		if (toggle_quotes(str[i], &sq, &dq, &i))
			continue ;
		if (str[i] == '*' && !sq && !dq)
			new[j++] = WILDCARD;
		else
			new[j++] = str[i];
		i++;
	}
	new[j] = '\0';
	return (new);
}

/**
 * @fn int wildcard_token_count(t_token *tokens)
 * @brief Counts the number of tokens in a list.
 * @details Helper function used to detect Ambiguous Redirects (count > 1).
 * @param tokens The list of tokens to count.
 * @return       The number of tokens.
 */
int	wildcard_token_count(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens)
	{
		count++;
		tokens = tokens->next;
	}
	return (count);
}
