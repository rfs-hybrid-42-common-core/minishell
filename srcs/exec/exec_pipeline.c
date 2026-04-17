/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:58:28 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/17 05:17:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn static bool setup_fork(pid_t *pid, int *p_fd)
 * @brief Helper to safely fork a new process.
 * @details Wraps the fork() system call. If the fork fails:
 * 1. Prints a "fork" error message to stderr.
 * 2. Closes the open pipe file descriptors to prevent leaks.
 * 3. Returns false to signal failure to the caller.
 * @param pid  Pointer to store the resulting Process ID.
 * @param p_fd Array of pipe file descriptors (used for cleanup on error).
 * @return     true if fork succeeded, false otherwise.
 */
static bool	setup_fork(pid_t *pid, int *p_fd)
{
	*pid = fork();
	if (*pid == -1)
	{
		perror("minishell: fork");
		close(p_fd[0]);
		close(p_fd[1]);
		return (false);
	}
	return (true);
}

/**
 * @fn static void exec_left_child(t_mini *mini, t_ast *lnode, int *p_fd)
 * @brief Executes the left side of the pipeline (Writer).
 * @details 
 * 1. Closes unused read-end of the pipe.
 * 2. Redirects STDOUT to the write-end of the pipe.
 * 3. Executes the command.
 * 4. Cleans up memory if exec or dup2 fails.
 * @param mini  Pointer to the main minishell structure.
 * @param lnode The AST node for the left command.
 * @param p_fd  The pipe file descriptors.
 */
static void	exec_left_child(t_mini *mini, t_ast *lnode, int *p_fd)
{
	close(p_fd[0]);
	if (dup2(p_fd[1], STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(p_fd[1]);
		cleanup_iteration(mini);
		free_mini(mini);
		exit(EXIT_FAILURE);
	}
	close(p_fd[1]);
	signal(SIGPIPE, SIG_IGN);
	mini->exit_code = exec(mini, lnode);
	cleanup_iteration(mini);
	free_mini(mini);
	exit(mini->exit_code);
}

/**
 * @fn static void exec_right_child(t_mini *mini, t_ast *rnode, int *p_fd)
 * @brief Executes the right side of the pipeline (Reader).
 * @details 
 * 1. Closes unused write-end of the pipe.
 * 2. Redirects STDIN to the read-end of the pipe.
 * 3. Executes the command.
 * 4. Cleans up memory if exec or dup2 fails.
 * @param mini  Pointer to the main minishell structure.
 * @param rnode The AST node for the right command.
 * @param p_fd  The pipe file descriptors.
 */
static void	exec_right_child(t_mini *mini, t_ast *rnode, int *p_fd)
{
	close(p_fd[1]);
	if (dup2(p_fd[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(p_fd[0]);
		cleanup_iteration(mini);
		free_mini(mini);
		exit(EXIT_FAILURE);
	}
	close(p_fd[0]);
	mini->exit_code = exec(mini, rnode);
	cleanup_iteration(mini);
	free_mini(mini);
	exit(mini->exit_code);
}

/**
 * @fn static int wait_and_check_newline(pid_t pid[2])
 * @brief Waits for pipeline children and manages newline printing.
 * @details 
 * 1. Calls waitpid for both the left and right child processes.
 * 2. Checks if left child exited via SIGINT (130) or died by SIGPIPE (13). 
 * (SIGPIPE happens if left tries to print the SIGINT newline to closed pipe).
 * 3. If so, manually prints newline unless right child also handled SIGINT.
 * @param pid Array of two PIDs corresponding to the left and right children.
 * @return    The exit status of the right-hand process.
 */
static int	wait_and_check_newline(pid_t pid[2])
{
	int	lnode_status;
	int	rnode_status;

	waitpid(pid[0], &lnode_status, 0);
	waitpid(pid[1], &rnode_status, 0);
	if ((WIFEXITED(lnode_status) && WEXITSTATUS(lnode_status) == EXIT_SIGINT)
		|| (WIFSIGNALED(lnode_status) && WTERMSIG(lnode_status) == SIGPIPE))
	{
		if (!(WIFEXITED(rnode_status)
				&& WEXITSTATUS(rnode_status) == EXIT_SIGINT))
			write(STDOUT_FILENO, "\n", 1);
	}
	setup_signals();
	return (rnode_status);
}

/**
 * @fn int exec_pipeline(t_mini *mini, t_ast *lnode, t_ast *rnode)
 * @brief Handles the pipe operator '|'.
 * @details Creates a pipe, forks two children (Left then Right), and waits.
 * - Uses `wait_and_check_newline` to handle signal synchronization and 
 * exit status retrieval.
 * - Returns the exit status of the Right child (as per Bash standard).
 * @param mini  Pointer to the main minishell structure.
 * @param lnode The AST node on the left side.
 * @param rnode The AST node on the right side.
 * @return      The exit status of the pipeline (right command).
 */
int	exec_pipeline(t_mini *mini, t_ast *lnode, t_ast *rnode)
{
	pid_t	pid[2];
	int		p_fd[2];

	if (pipe(p_fd) == -1)
		return (perror("minishell: pipe"), EXIT_FAILURE);
	if (!setup_fork(&pid[0], p_fd))
		return (EXIT_FAILURE);
	if (pid[0] == 0)
		exec_left_child(mini, lnode, p_fd);
	if (!setup_fork(&pid[1], p_fd))
	{
		kill(pid[0], SIGTERM);
		waitpid(pid[0], NULL, 0);
		return (EXIT_FAILURE);
	}
	if (pid[1] == 0)
		exec_right_child(mini, rnode, p_fd);
	close(p_fd[0]);
	close(p_fd[1]);
	setup_parent_signals();
	return (exec_exit_status(wait_and_check_newline(pid)));
}
