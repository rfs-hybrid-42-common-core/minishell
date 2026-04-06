/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 23:00:26 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 16:16:02 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/**
 * @fn void print_export_error(char *arg, char *msg)
 * @brief Prints a specific error message for the export command.
 * @details Formats the error as: minishell: export: `ARG': MSG. 
 * Matches the exact behavior of Bash for invalid identifiers.
 * @param arg The invalid argument string.
 * @param msg The error description (e.g., "not a valid identifier").
 */
void	print_export_error(char *arg, char *msg)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("': ", STDERR_FILENO);
	ft_putendl_fd(msg, STDERR_FILENO);
}

/**
 * @fn void print_command_error(char *cmd, char *msg)
 * @brief Prints a generic command error message.
 * @details Formats the error as: minishell: CMD: MSG. 
 * Used for "command not found", "permission denied", etc.
 * @param cmd The command name that failed (optional).
 * @param msg The error message description.
 */
void	print_command_error(char *cmd, char *msg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putendl_fd(msg, STDERR_FILENO);
}

/**
 * @fn void print_syntax_error(char *msg, char *token)
 * @brief Prints a standardized syntax error message.
 * @details Formats the error to match Bash's exact output: 
 * `minishell: syntax error <msg> '<token>'`. Commonly used for 
 * misplaced operators or unclosed quotes.
 * @param msg   The specific error description.
 * @param token The literal token string causing the issue (can be NULL).
 */
void	print_syntax_error(char *msg, char *token)
{
	ft_putstr_fd("minishell: syntax error ", STDERR_FILENO);
	ft_putstr_fd(msg, STDERR_FILENO);
	if (token)
	{
		ft_putstr_fd(" `", STDERR_FILENO);
		ft_putstr_fd(token, STDERR_FILENO);
		ft_putstr_fd("'", STDERR_FILENO);
	}
	ft_putstr_fd("\n", STDERR_FILENO);
}

/**
 * @fn void print_write_error(char *cmd)
 * @brief Prints a standard Bash write error message.
 * @details Formats as: minishell: CMD: write error: [strerror]. 
 * Uses errno to fetch the system error string (e.g., "Broken pipe").
 * IMPORTANT: Ignores EPIPE (Broken Pipe) errors to match Bash behavior 
 * (e.g., "yes | head" should not print "yes: write error: Broken pipe").
 * @param cmd The command name (e.g., "echo" or "pwd").
 */
void	print_write_error(char *cmd)
{
	if (errno == EPIPE)
		return ;
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd(": write error: ", STDERR_FILENO);
	ft_putendl_fd(strerror(errno), STDERR_FILENO);
}

/**
 * @fn void print_end_error(t_token *token)
 * @brief Prints a syntax error regarding unexpected tokens at the end of input.
 * @details Formats the specific error needed when a command terminates 
 * abruptly, typically used when redirections have no target file.
 * @param token The token representing the point of failure.
 */
void	print_end_error(t_token *token)
{
	if (!token || token->type == TOKEN_END)
		print_syntax_error("near unexpected token", "newline");
}
