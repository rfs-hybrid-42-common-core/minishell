/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 17:01:36 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/17 14:37:16 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"

/**
 * @fn static void sigint_handler(int sig)
 * @brief Handler for SIGINT (Ctrl+C) in interactive mode.
 * @details 
 * 1. Sets the global signal flag to SIGINT.
 * 2. Writes a newline to keep output clean.
 * 3. Uses readline functions to clear the current line and regenerate the 
 * prompt on a new line.
 * @param sig The signal number (unused, always SIGINT).
 */
static void	sigint_handler(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * @fn void setup_parent_signals(void)
 * @brief Sets up signals for the Parent process during command execution.
 * @details 
 * - SIGINT (Ctrl+C): Ignored (SIG_IGN). The parent waits for the child.
 * - SIGQUIT (Ctrl+\): Ignored (SIG_IGN). Preventing core dumps in the shell.
 * - SIGPIPE: Ignored (SIG_IGN). Prevents parent crash on broken pipes.
 */
void	setup_parent_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
}

/**
 * @fn void setup_child_signals(void)
 * @brief Sets up signals for Child processes.
 * @details Restores default behavior (SIG_DFL) for SIGINT, SIGQUIT, and 
 * SIGPIPE.
 * - SIGINT/SIGQUIT: Ensures child commands can be killed by the user.
 * - SIGPIPE: Ensures commands writing to broken pipes (e.g., ls | invalid_cmd) 
 * are silently killed by the OS (strict Bash parity), preventing them from 
 * failing on write() and printing "Broken pipe" to STDERR.
 */
void	setup_child_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
}

/**
 * @fn void setup_signals(void)
 * @brief Sets up signals for Interactive Mode (Prompt).
 * @details 
 * - SIGINT (Ctrl+C): Uses custom handler to reset prompt.
 * - SIGQUIT (Ctrl+\): Ignored to prevent the shell from dumping core.
 * - SIGPIPE: Ignored so the parent shell does not crash if an internal 
 * builtin attempts to write to a closed pipeline.
 * Used when the shell is waiting for user input via readline.
 */
void	setup_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = &sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		exit(EXIT_FAILURE);
	}
}
