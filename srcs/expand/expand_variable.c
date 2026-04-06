/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 01:08:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:43:46 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn static char *process_dollar(t_mini *mini, char *str, int *i, bool dq)
 * @brief Handles the logic when a '$' sign is encountered.
 * @details 
 * 1. Checks for empty quotes (e.g., $"" or $'') when not inside double quotes. 
 * If found, the '$' is skipped (ignored) to match Bash behavior.
 * 2. Checks if the '$' is followed by a valid variable character, '?', or '$'. 
 * If so, calls `get_var_value` to retrieve the expansion.
 * 3. Otherwise, treats the '$' as a literal character.
 * @param mini Pointer to the main shell structure.
 * @param str  The string being parsed.
 * @param i    Pointer to the current index (updated during processing).
 * @param dq   Boolean flag indicating if we are inside double quotes.
 * @return     The expanded string segment, or NULL if the '$' was ignored.
 */
static char	*process_dollar(t_mini *mini, char *str, int *i, bool dq)
{
	if (!dq && (str[*i + 1] == '\'' || str[*i + 1] == '\"'))
	{
		(*i)++;
		return (NULL);
	}
	if (ft_isalnum(str[*i + 1]) || str[*i + 1] == '_'
		|| str[*i + 1] == '?' || str[*i + 1] == '$')
		return (get_var_value(mini, str, i));
	(*i)++;
	return (ft_strdup("$"));
}

/**
 * @fn static char *perform_expansion(t_mini *mini, char *str)
 * @brief Scans a string and replaces all $VAR occurrences.
 * @details Handles quoting rules: variables inside single quotes are ignored.
 * - Iterates through the string, toggling quote states.
 * - Handles backslash escapes for characters that need them.
 * - Delegates variable expansion detection ($) to `process_dollar`.
 * - Appends literal characters to the result string.
 * @param mini Pointer to main shell structure.
 * @param str  The original string.
 * @return     The fully expanded string.
 */
static char	*perform_expansion(t_mini *mini, char *str)
{
	char	*new;
	bool	sq;
	bool	dq;
	int		i;

	new = NULL;
	sq = false;
	dq = false;
	i = 0;
	while (str[i])
	{
		if (toggle_quotes(str[i], &sq, &dq, &i))
			i--;
		if (str[i] == '\\' && !sq && str[i + 1])
		{
			new = append_char(new, str[i++]);
			new = append_char(new, str[i++]);
		}
		else if (str[i] == '$' && !sq)
			new = append_str(new, process_dollar(mini, str, &i, dq));
		else
			new = append_char(new, str[i++]);
	}
	return (new);
}

/**
 * @fn static bool check_heredoc_quotes(t_token *curr, t_token *next)
 * @brief Checks if the delimiter of a Here-Document is quoted.
 * @details If the current token is a HEREDOC operator (<<), checks the 
 * next token (the delimiter) for any single or double quotes.
 * This distinction determines whether variables inside the heredoc should 
 * be expanded (unquoted delimiter) or treated literally (quoted delimiter).
 * @param curr The current token being processed.
 * @param next The token immediately following 'curr' (the delimiter).
 * @return     true if quotes are found in the delimiter, false otherwise.
 */
static bool	check_heredoc_quotes(t_token *curr, t_token *next)
{
	int	i;

	if (curr->type == TOKEN_HEREDOC && next && next->type == TOKEN_WORD
		&& next->val)
	{
		i = 0;
		while (next->val[i])
		{
			if (next->val[i] == '\'' || next->val[i] == '\"')
				return (true);
			i++;
		}
	}
	return (false);
}

/**
 * @fn static bool check_ambiguous(char *str)
 * @brief Checks if a string causes an ambiguous redirect.
 * @details Used when a variable expands inside a redirection filename.
 * - Returns true if the string is empty.
 * - Returns true if the string contains unquoted spaces (which would mean 
 * it expands to multiple arguments).
 * @param str The expanded string to check.
 * @return    true if ambiguous, false if valid.
 */
static bool	check_ambiguous(char *str)
{
	int		i;
	bool	sq;
	bool	dq;

	if (!str || !*str)
		return (true);
	i = 0;
	sq = false;
	dq = false;
	while (str[i])
	{
		if (toggle_quotes(str[i], &sq, &dq, &i))
			continue ;
		if (!sq && !dq && ft_isspace(str[i]))
			return (true);
		i++;
	}
	return (false);
}

/**
 * @fn bool expand_variable(t_mini *mini, t_token **curr, t_token *prev, 
 * t_token *next)
 * @brief Handles variable expansion and ambiguous redirect detection.
 * @details 
 * 1. Checks if current token is a WORD with '$'.
 * 2. Calls `perform_expansion` to resolve variables.
 * 3. **Ambiguity Check**: If `prev` is a redirection operator (<, >, >>) and 
 * the expanded result is ambiguous (empty or multiple words), it prints an 
 * error and returns `false`.
 * 4. Calls `expand_split` to handle Word Splitting.
 * 5. Checks for HEREDOC quotes.
 * @param mini Pointer to main shell structure.
 * @param curr Pointer to the address of the current token.
 * @param prev The previous token (used to detect redirection context).
 * @param next The next token (used for heredoc delimiter check).
 * @return     true on success, false if ambiguous redirect error occurred.
 */
bool	expand_variable(t_mini *mini, t_token **curr, t_token *prev,
	t_token *next)
{
	char	*expanded;

	if ((*curr)->type == TOKEN_WORD && (*curr)->val
		&& ft_strchr((*curr)->val, '$'))
	{
		expanded = perform_expansion(mini, (*curr)->val);
		if (prev && (prev->type == TOKEN_REDIR_IN
				|| prev->type == TOKEN_REDIR_OUT
				|| prev->type == TOKEN_APPEND) && check_ambiguous(expanded))
		{
			print_command_error((*curr)->val, "ambiguous redirect");
			free(expanded);
			return (false);
		}
		if (expanded)
		{
			expand_split(curr, expanded);
			free(expanded);
		}
	}
	if (check_heredoc_quotes(*curr, next))
		(*curr)->type = TOKEN_HEREDOC_QUOTED;
	return (true);
}
