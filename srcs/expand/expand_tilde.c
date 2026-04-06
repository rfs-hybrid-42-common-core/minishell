/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tilde.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 17:26:19 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:30:12 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand.h"

/**
 * @fn void expand_tilde(t_mini *mini, t_token *token)
 * @brief Expands the tilde (~) to the user's home directory.
 * @details 
 * 1. Checks if the token is a WORD and starts with '~'.
 * 2. Validates that it matches `~` or `~/...` (to avoid expanding `~user`).
 * 3. Retrieves the "HOME" environment variable.
 * 4. If HOME exists, replaces `~` with the path value using `ft_strjoin`.
 * 5. If HOME is unset, the token remains unchanged (standard Minishell logic).
 * @param mini  Pointer to the main shell structure.
 * @param token The token to potentially expand.
 */
void	expand_tilde(t_mini *mini, t_token *token)
{
	t_env	*env;
	char	*tmp;

	if (token->type != TOKEN_WORD || !token->val || token->val[0] != '~')
		return ;
	if (token->val[1] != '\0' && token->val[1] != '/')
		return ;
	env = find_env(mini->env, "HOME");
	if (env && env->val)
	{
		tmp = ft_strjoin(env->val, token->val + 1);
		free(token->val);
		token->val = tmp;
	}
}
