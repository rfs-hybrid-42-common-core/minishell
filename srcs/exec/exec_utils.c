/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:03:27 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:28:54 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn int exec_exit_status(int status)
 * @brief Converts waitpid status to shell exit code.
 * @details Extracts the exit code (WEXITSTATUS) or signal number (WTERMSIG). 
 * Prints newline or "Quit" messages if the process was killed by a signal.
 * @param status The raw integer returned by waitpid.
 * @return       The standardized shell exit code (0-255).
 */
int	exec_exit_status(int status)
{
	int	sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (sig == SIGQUIT)
			ft_putendl_fd("Quit (core dumped)", STDOUT_FILENO);
		return (128 + sig);
	}
	return (EXIT_FAILURE);
}

/**
 * @fn bool is_valid_key(char *key)
 * @brief Checks if a string starts with a valid environment variable key.
 * @details Validates that the key:
 * 1. Starts with an alphabetic character or underscore.
 * 2. Contains only alphanumeric characters or underscores thereafter.
 * 3. Stops validation at the end of the string OR at the first '=' sign. 
 * This allows it to validate both "VAR" (for export) and "VAR=val" 
 * (for assignment).
 * @param key The string to check.
 * @return    true if the key is valid, false otherwise.
 */
bool	is_valid_key(char *key)
{
	int	i;

	if (!key || (!ft_isalpha(key[0]) && key[0] != '_'))
		return (false);
	i = 0;
	while (key[++i] && key[i] != '=')
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (false);
	return (true);
}

/**
 * @fn bool is_directory(char *path)
 * @brief Checks if a path points to a directory.
 * @details Used to trigger specific "Is a directory" errors during execution.
 * @param path The file path to check.
 * @return     true if it is a directory, false otherwise.
 */
bool	is_directory(char *path)
{
	struct stat	buf;

	if (stat(path, &buf))
		return (false);
	return (S_ISDIR(buf.st_mode));
}
