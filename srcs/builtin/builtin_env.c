/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:02:38 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:17:12 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn int builtin_env(t_mini *mini, char **argv)
 * @brief Prints the environment variables.
 * @details Iterates through the shell's environment list and prints every 
 * variable that has both a key and a value (and implicitly is exported) in the 
 * format "KEY=VALUE".
 * - Uses write() to safely handle output errors (e.g. full disk).
 * - Explicitly rejects arguments to match subject requirements.
 * @param mini Pointer to the main minishell structure.
 * @param argv Command arguments.
 * @return     EXIT_SUCCESS on success, or EXIT_FAILURE on error.
 */
int	builtin_env(t_mini *mini, char **argv)
{
	t_env	*env;

	if (argv[1])
	{
		print_command_error("env", "too many arguments");
		return (EXIT_FAILURE);
	}
	env = mini->env;
	while (env)
	{
		if (env->exported && env->val)
		{
			if (write(STDOUT_FILENO, env->key, ft_strlen(env->key)) < 0
				|| write(STDOUT_FILENO, "=", 1) < 0
				|| write(STDOUT_FILENO, env->val, ft_strlen(env->val)) < 0
				|| write(STDOUT_FILENO, "\n", 1) < 0)
				return (print_write_error("env"), EXIT_FAILURE);
		}
		env = env->next;
	}
	return (EXIT_SUCCESS);
}
