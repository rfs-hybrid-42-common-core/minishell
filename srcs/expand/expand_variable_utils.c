/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 23:25:28 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:31:04 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn char *append_str(char *s1, char *s2)
 * @brief Concatenates two strings and frees the inputs.
 * @details Wrapper for ft_strjoin that handles NULL inputs gracefully and 
 * automatically frees both source strings. Used extensively during variable 
 * expansion value concatenation.
 * @param s1 The first string (will be freed).
 * @param s2 The second string (will be freed).
 * @return   Allocated new string s1+s2.
 */
char	*append_str(char *s1, char *s2)
{
	char	*new;

	if (!s1)
		s1 = ft_strdup("");
	if (!s2)
		s2 = ft_strdup("");
	new = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	return (new);
}

/**
 * @fn static char *get_special_var(t_mini *mini, char *str, int *index)
 * @brief Helper to handle special variables $?, $$, and $0.
 * @details 
 * - $?: Returns exit_code.
 * - $$: Returns PID.
 * - $0: Returns "minishell".
 * @param mini  Pointer to the main shell structure.
 * @param str   The string being parsed.
 * @param index Pointer to current index (incremented).
 * @return      Allocated string with the value.
 */
static char	*get_special_var(t_mini *mini, char *str, int *index)
{
	if (str[*index] == '?')
	{
		(*index)++;
		return (ft_itoa(mini->exit_code));
	}
	if (str[*index] == '$')
	{
		(*index)++;
		return (ft_strdup(mini->pid_str));
	}
	(*index)++;
	return (ft_strdup("minishell"));
}

/**
 * @fn static char *escape_special_chars(char *str)
 * @brief Escapes quotes and backslashes in a string.
 * @details Used to protect variable values from being interpreted as syntax 
 * during the subsequent quote removal phase.
 * - Turns `\` into `\\`
 * - Turns `'` into `\'`
 * - Turns `"` into `\"`
 * @param str The raw variable value.
 * @return    A new allocated string with escaped characters.
 */
static char	*escape_special_chars(char *str)
{
	char	*new;
	int		i;

	if (!str)
		return (NULL);
	new = NULL;
	i = 0;
	while (str[i])
	{
		if (str[i] == '\\' || str[i] == '\'' || str[i] == '\"')
			new = append_char(new, '\\');
		new = append_char(new, str[i++]);
	}
	return (new);
}

/**
 * @fn char *get_var_value(t_mini *mini, char *str, int *index)
 * @brief Retrieves the value of a specific environment variable.
 * @details Handles special cases via helper, standard env vars, and lone $. 
 * Calls escape_special_chars on the result to prevent the expansion from being 
 * re-interpreted by quote removal logic.
 * @param mini  Pointer to main shell structure.
 * @param str   The string being parsed.
 * @param index Pointer to current index (starts at '$').
 * @return      Allocated string containing the variable's value.
 */
char	*get_var_value(t_mini *mini, char *str, int *index)
{
	t_env	*env;
	char	*key;
	int		start;

	(*index)++;
	if (str[*index] == '?' || str[*index] == '$' || str[*index] == '0')
		return (get_special_var(mini, str, index));
	start = *index;
	while (str[*index] && (ft_isalnum(str[*index]) || str[*index] == '_'))
		(*index)++;
	if (start == *index)
		return (ft_strdup("$"));
	key = ft_substr(str, start, *index - start);
	env = find_env(mini->env, key);
	free(key);
	if (env && env->val)
		return (escape_special_chars(env->val));
	return (ft_strdup(""));
}
