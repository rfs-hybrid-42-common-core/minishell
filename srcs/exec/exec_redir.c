/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:57:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:28:33 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn static int open_file(char *file, t_token_type token)
 * @brief Opens a file with specific flags based on the redirection type.
 * @details 
 * - REDIR_IN (<): Opens for reading (O_RDONLY).
 * - REDIR_OUT (>): Opens for writing, creates if missing, truncates (O_TRUNC).
 * - APPEND (>>): Opens for writing, creates if missing, appends (O_APPEND).
 * @param file  The path of the file to open.
 * @param token The type of redirection token.
 * @return      The file descriptor on success, or -1 on failure.
 */
static int	open_file(char *file, t_token_type token)
{
	int	fd;

	fd = -1;
	if (token == TOKEN_REDIR_IN)
		fd = open(file, O_RDONLY);
	if (token == TOKEN_REDIR_OUT)
		fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (token == TOKEN_APPEND)
		fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0644);
	return (fd);
}

/**
 * @fn static bool check_fd(int fd, char *file)
 * @brief Validates the file descriptor after an open() attempt.
 * @details If the file descriptor is -1, it prints a system error message 
 * using print_command_error() to ensure the "minishell:" prefix is present.
 * @param fd   The file descriptor to check.
 * @param file The filename (used for error reporting).
 * @return     true if valid, false if invalid (-1).
 */
static bool	check_fd(int fd, char *file)
{
	if (fd == -1)
	{
		print_command_error(file, strerror(errno));
		return (false);
	}
	return (true);
}

/**
 * @fn static bool perform_duplicate(int *fd, t_token_type token)
 * @brief Replaces Standard Input or Output with the new file descriptor.
 * @details Uses dup2() to copy the file descriptor to:
 * - STDIN_FILENO for input redirections (<, <<, quoted <<).
 * - STDOUT_FILENO for output redirections (>, >>).
 * Closes the original fd if dup2 fails.
 * @param fd    Pointer to the file descriptor (used to close it on error).
 * @param token The type of redirection (includes TOKEN_HEREDOC_QUOTED).
 * @return      true on success, false on dup2 failure.
 */
static bool	perform_duplicate(int *fd, t_token_type token)
{
	if (token == TOKEN_REDIR_IN || token == TOKEN_HEREDOC
		|| token == TOKEN_HEREDOC_QUOTED)
	{
		if (dup2(*fd, STDIN_FILENO) == -1)
		{
			close(*fd);
			return (false);
		}
	}
	else
	{
		if (dup2(*fd, STDOUT_FILENO) == -1)
		{
			close(*fd);
			return (false);
		}
	}
	return (true);
}

/**
 * @fn int setup_redirections(t_redir *redirs)
 * @brief Processes input/output redirections for a command.
 * @details Iterates through the redirection list, opens files with correct 
 * flags, and uses dup2 to replace STDIN or STDOUT. Handles heredoc files 
 * (both quoted and unquoted) by unlinking them immediately after opening to 
 * ensure cleanup.
 * @param redirs Pointer to the linked list of redirections.
 * @return       EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int	setup_redirections(t_redir *redirs)
{
	int		fd;
	t_redir	*curr;

	curr = redirs;
	while (curr)
	{
		if (curr->type == TOKEN_HEREDOC || curr->type == TOKEN_HEREDOC_QUOTED)
		{
			fd = open(curr->file, O_RDONLY);
			if (fd != -1)
				unlink(curr->file);
		}
		else
			fd = open_file(curr->file, curr->type);
		if (!check_fd(fd, curr->file)
			|| !perform_duplicate(&fd, curr->type))
			return (EXIT_FAILURE);
		close(fd);
		curr = curr->next;
	}
	return (EXIT_SUCCESS);
}
