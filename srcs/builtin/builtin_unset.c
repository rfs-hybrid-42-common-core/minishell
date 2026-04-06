/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:01:38 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:19:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn static void unset_env_node(t_mini *mini, char *key)
 * @brief Helper to remove a single variable from the environment list.
 * @details Traverses the linked list looking for a node with a matching key. 
 * If found, it unlinks the node from the list, frees the key, value, and the 
 * node itself.
 * @param mini Pointer to the main minishell structure.
 * @param key  The key of the variable to remove.
 */
static void	unset_env_node(t_mini *mini, char *key)
{
	t_env	*curr;
	t_env	*prev;

	curr = mini->env;
	prev = NULL;
	while (curr)
	{
		if (!ft_strcmp(curr->key, key))
		{
			if (prev)
				prev->next = curr->next;
			else
				mini->env = curr->next;
			free(curr->key);
			free(curr->val);
			free(curr);
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

/**
 * @fn int builtin_unset(t_mini *mini, char **argv)
 * @brief Removes variables from the environment.
 * @details Loops through all arguments provided to 'unset'.
 * - If `argv[1]` starts with `-`, we reject it as an unsupported option.
 * - Otherwise, we assume all arguments are variable names (even if they start 
 * with `-` later), matching Bash behavior (e.g., `unset VAR -n`).
 * @param mini Pointer to the main minishell structure.
 * @param argv Command arguments (list of keys to unset).
 * @return     EXIT_SUCCESS on success, EXIT_FAILURE if invalid option found.
 */
int	builtin_unset(t_mini *mini, char **argv)
{
	int	i;

	if (argv[1] && argv[1][0] == '-')
	{
		print_command_error("unset", "options not supported");
		return (EXIT_FAILURE);
	}
	i = 0;
	while (argv[++i])
		unset_env_node(mini, argv[i]);
	return (EXIT_SUCCESS);
}
