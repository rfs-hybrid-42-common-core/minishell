/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_subshell.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:58:50 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:28:39 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn int exec_subshell(t_mini *mini, t_ast *ast)
 * @brief Executes a command block inside a subshell '()'.
 * @details Forks a child process to run the enclosed AST tree.
 * - If redirections are attached to the subshell (e.g., "(ls) > out"), they 
 * are applied inside the child before execution logic starts.
 * - CRITICAL: Frees all memory (AST, Env) before exiting to prevent leaks.
 * @param mini Pointer to the main minishell structure.
 * @param ast  The root of the AST subtree to run in the subshell.
 * @return     The exit status of the subshell.
 */
int	exec_subshell(t_mini *mini, t_ast *ast)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"), EXIT_FAILURE);
	if (pid == 0)
	{
		setup_child_signals();
		if (ast->cmd && ast->cmd->redirs
			&& setup_redirections(ast->cmd->redirs))
		{
			cleanup_iteration(mini);
			free_mini(mini);
			exit(EXIT_FAILURE);
		}
		status = exec(mini, ast->lnode);
		cleanup_iteration(mini);
		free_mini(mini);
		exit(status);
	}
	setup_parent_signals();
	waitpid(pid, &status, 0);
	setup_signals();
	return (exec_exit_status(status));
}
