/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:01:21 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 03:51:21 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn int builtin_pwd(t_mini *mini, char **argv)
 * @brief Handles the 'pwd' builtin command.
 * @details Prints the current working directory to STDOUT.
 * Priority logic:
 * 1. Checks the 'PWD' environment variable. If set, prints it.
 * 2. If 'PWD' is unset, calls getcwd() to fetch the physical path.
 * 3. Uses write() directly to detect output errors (e.g., broken pipe).
 * @param mini Pointer to the main shell structure.
 * @param argv Command arguments (unused).
 * @return     EXIT_SUCCESS on success, or EXIT_FAILURE on error.
 */
int	builtin_pwd(t_mini *mini, char **argv)
{
	t_env	*pwd;
	char	*cwd;

	(void) argv;
	pwd = find_env(mini->env, "PWD");
	if (pwd && pwd->val)
	{
		if (write(STDOUT_FILENO, pwd->val, ft_strlen(pwd->val)) < 0
			|| write(STDOUT_FILENO, "\n", 1) < 0)
			return (print_write_error("pwd"), EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		print_command_error("pwd", "error retrieving current directory");
		return (EXIT_FAILURE);
	}
	if (write(STDOUT_FILENO, cwd, ft_strlen(cwd)) < 0
		|| write(STDOUT_FILENO, "\n", 1) < 0)
		return (free(cwd), print_write_error("pwd"), EXIT_FAILURE);
	free(cwd);
	return (EXIT_SUCCESS);
}
