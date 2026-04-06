/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcard.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 01:08:52 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:45:12 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn static bool wildcard_recursion(char *wc_str, char *file)
 * @brief Recursive function to match a filename against a pattern.
 * @details Standard globbing algorithm:
 * - If both end: match.
 * - If '*': try matching 0 or more characters (recurse).
 * - If char match: advance both.
 * @param wc_str The pattern string (can contain WILDCARD sentinel).
 * @param file   The directory entry name to check.
 * @return       true if it matches, false otherwise.
 */
static bool	wildcard_recursion(char *wc_str, char *file)
{
	if (*wc_str == '\0' && *file == '\0')
		return (true);
	if (*wc_str == WILDCARD)
	{
		if (wildcard_recursion(wc_str + 1, file))
			return (true);
		if (*file && wildcard_recursion(wc_str, file + 1))
			return (true);
		return (false);
	}
	if (*wc_str == *file)
		return (wildcard_recursion(wc_str + 1, file + 1));
	return (false);
}

/**
 * @fn static t_token *get_wildcard_tokens(char *wc_str)
 * @brief Reads the current directory and finds matching files.
 * @details Opens ".", iterates through all entries, and applies filtering:
 * - **Dotfiles**: Skips `.` and `..` explicitly.
 * - **Hidden Files**: Skips files starting with `.` unless the pattern 
 * `wc_str` also starts with `.` (matching Bash behavior).
 * - **Recursion**: Calls `wildcard_recursion` to check validity. 
 * If a match is found, creates a new token. Sorts the final result 
 * alphabetically.
 * @param wc_str The pattern string.
 * @return       Head of the list of matching tokens, or NULL.
 */
static t_token	*get_wildcard_tokens(char *wc_str)
{
	DIR				*dir;
	struct dirent	*entry;
	t_token			*wc_tokens;
	t_token			*new;

	wc_tokens = NULL;
	dir = opendir(".");
	if (!dir)
		return (NULL);
	entry = readdir(dir);
	while (entry)
	{
		if (ft_strcmp(entry->d_name, ".") && ft_strcmp(entry->d_name, "..")
			&& !(*(entry->d_name) == '.' && *wc_str != '.')
			&& wildcard_recursion(wc_str, entry->d_name))
		{
			new = token_new(TOKEN_WORD, ft_strdup(entry->d_name));
			if (new)
				token_add_back(&wc_tokens, new);
		}
		entry = readdir(dir);
	}
	closedir(dir);
	token_sort(&wc_tokens);
	return (wc_tokens);
}

/**
 * @fn static bool ambiguous_redirect(t_token *tokens, t_token *curr, 
 * t_token *prev, bool *error)
 * @brief Checks for Ambiguous Redirect errors.
 * @details If a wildcard is used in a redirection context (>, >>, <) AND 
 * it expands to more than one file, Bash considers this an error.
 * Prints "ambiguous redirect" to STDERR and sets the error flag.
 * @param tokens The list of expanded tokens (matches).
 * @param curr   The original token (containing the wildcard pattern).
 * @param prev   The previous token (to check for redirection operators).
 * @param error  Pointer to the error flag.
 * @return       true if ambiguous, false otherwise.
 */
static bool	ambiguous_redirect(t_token *tokens, t_token *curr, t_token *prev,
	bool *error)
{
	if (prev && (prev->type == TOKEN_REDIR_IN || prev->type == TOKEN_REDIR_OUT
			|| prev->type == TOKEN_APPEND) && wildcard_token_count(tokens) > 1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(curr->val, STDERR_FILENO);
		ft_putendl_fd(": ambiguous redirect", STDERR_FILENO);
		*error = true;
		return (true);
	}
	return (false);
}

/**
 * @fn t_token *expand_wildcard(t_token **tokens, t_token *curr, 
 * t_token **prev, bool *error)
 * @brief Replaces a wildcard token with matching filenames.
 * @details 
 * 1. Checks if token has a wildcard.
 * 2. Generates the pattern string.
 * 3. Finds matches in current directory.
 * 4. Checks for Ambiguous Redirects (if > 1 match in redirection).
 * 5. If matches found, replaces the single wildcard token with the list.
 * @param tokens Double pointer to head of token list.
 * @param curr   The current wildcard token.
 * @param prev   Double pointer to previous token (to fix linking).
 * @param error  Pointer to error flag (set true on ambiguous redirect).
 * @return       Pointer to the last token inserted, or NULL if no expansion.
 */
t_token	*expand_wildcard(t_token **tokens, t_token *curr, t_token **prev,
	bool *error)
{
	char	*str;
	t_token	*wc_tokens;
	t_token	*last;

	if (!has_wildcard(curr))
		return (NULL);
	str = get_wildcard_str(curr->val);
	if (!str)
		return (NULL);
	wc_tokens = get_wildcard_tokens(str);
	free(str);
	if (!wc_tokens)
		return (NULL);
	if (ambiguous_redirect(wc_tokens, curr, *prev, error))
		return (free_tokens(&wc_tokens), NULL);
	last = wc_tokens;
	while (last->next)
		last = last->next;
	last->next = curr->next;
	if (*prev)
		(*prev)->next = wc_tokens;
	else
		*tokens = wc_tokens;
	return (free(curr->val), free(curr), last);
}
