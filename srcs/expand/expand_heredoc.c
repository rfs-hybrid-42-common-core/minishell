/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 03:08:05 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:29:15 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn char *expand_heredoc(t_mini *mini, char *str)
 * @brief Performs variable expansion for Here-Document content.
 * @details Unlike standard shell expansion, this function treats single and 
 * double quotes as literal characters. It only looks for the `$` sign to 
 * trigger expansion of environment variables.
 * @param mini Pointer to the main shell structure (for env lookup).
 * @param str  The line read from the heredoc.
 * @return     The allocated string with variables expanded.
 */
char	*expand_heredoc(t_mini *mini, char *str)
{
	char	*new;
	int		i;

	new = NULL;
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?' || str[i + 1] == '$'))
			new = append_str(new, get_var_value(mini, str, &i));
		else
			new = append_char(new, str[i++]);
	}
	return (new);
}
