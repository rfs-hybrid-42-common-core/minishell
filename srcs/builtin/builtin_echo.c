/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:01:13 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:16:36 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn static bool is_n_flag(char *arg)
 * @brief Checks if an argument is a valid '-n' flag.
 * @details Valid flags must start with '-' followed by one or more 'n' 
 * characters and nothing else (e.g., "-n", "-nn", "-nnnn").
 * @param arg The argument string to check.
 * @return    true if it is a valid flag, false otherwise.
 */
static bool	is_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (false);
	i = 1;
	while (arg[i] == 'n')
		i++;
	if (arg[i] != '\0')
		return (false);
	return (true);
}

/**
 * @fn int builtin_echo(t_mini *mini, char **argv)
 * @brief Handles the 'echo' builtin command.
 * @details Prints arguments to STDOUT.
 * - Supports '-n' option (suppress trailing newline).
 * - Handles multiple/repetitive flags (e.g., "echo -n -n hello").
 * - Checks for write errors (e.g., full disk) and prints proper error message.
 * @param mini Pointer to the main shell structure (unused).
 * @param argv Command arguments (argv[0] is "echo").
 * @return     EXIT_SUCCESS on success, or EXIT_FAILURE on write error.
 */
int	builtin_echo(t_mini *mini, char **argv)
{
	bool	no_newline;
	int		i;

	(void) mini;
	no_newline = false;
	i = 1;
	while (argv[i] && is_n_flag(argv[i]))
	{
		no_newline = true;
		i++;
	}
	while (argv[i])
	{
		if (write(STDOUT_FILENO, argv[i], ft_strlen(argv[i])) < 0)
			return (print_write_error("echo"), EXIT_FAILURE);
		if (argv[i + 1] && write(STDOUT_FILENO, " ", 1) < 0)
			return (print_write_error("echo"), EXIT_FAILURE);
		i++;
	}
	if (!no_newline && write(STDOUT_FILENO, "\n", 1) < 0)
		return (print_write_error("echo"), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
