/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:58:32 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:22:45 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "builtin.h"

/**
 * @fn static void save_io(t_io *io)
 * @brief Saves the current Standard Input and Output file descriptors.
 * @details Uses dup() to create copies of STDIN (0) and STDOUT (1).
 * These copies are stored in the t_io structure so they can be restored later, 
 * typically after a command with redirections has finished executing.
 * @param io Pointer to the t_io structure where the FD copies will be stored.
 */
static void	save_io(t_io *io)
{
	io->in = dup(STDIN_FILENO);
	io->out = dup(STDOUT_FILENO);
}

/**
 * @fn static void restore_io(t_io *io)
 * @brief Restores Standard Input and Output to their original state.
 * @details Uses dup2() to overwrite the current STDIN/STDOUT with the saved 
 * copies from the t_io structure. After restoration, it closes the saved 
 * copies to prevent file descriptor leaks.
 * @param io Pointer to the t_io structure containing the saved FDs.
 */
static void	restore_io(t_io *io)
{
	if (io->in != -1)
	{
		dup2(io->in, STDIN_FILENO);
		close(io->in);
	}
	if (io->out != -1)
	{
		dup2(io->out, STDOUT_FILENO);
		close(io->out);
	}
}

/**
 * @fn int exec_command(t_mini *mini, t_cmd *cmd)
 * @brief Main handler for simple commands (Leaf nodes).
 * @details Orchestrates the execution: saves IO, sets up redirections, 
 * checks for variable assignments or empty commands, runs built-ins 
 * or external commands, and finally restores IO.
 * @param mini Pointer to the main minishell structure.
 * @param cmd  Pointer to the command structure.
 * @return     Exit status of the command.
 */
int	exec_command(t_mini *mini, t_cmd *cmd)
{
	int		exit_code;
	t_io	io;

	if (!cmd)
		return (EXIT_SUCCESS);
	save_io(&io);
	if (setup_redirections(cmd->redirs))
		return (restore_io(&io), EXIT_FAILURE);
	if (!cmd->argv || !cmd->argv[0])
		return (restore_io(&io), EXIT_SUCCESS);
	if (is_local_assignment(cmd->argv))
		exit_code = exec_local_assignment(mini, cmd);
	else if (is_builtin(cmd->argv[0]))
	{
		update_env_var(mini, "_", cmd->argv[0]);
		if (!ft_strcmp(cmd->argv[0], "exit"))
			return (restore_io(&io), exec_builtin(mini, cmd));
		exit_code = exec_builtin(mini, cmd);
	}
	else
		exit_code = exec_external(mini, cmd, &io);
	return (restore_io(&io), exit_code);
}
