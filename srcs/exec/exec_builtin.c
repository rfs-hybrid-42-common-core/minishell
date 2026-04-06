/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 22:53:22 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:20:02 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "builtin.h"

/**
 * @fn int exec_builtin(t_mini *mini, t_cmd *cmd)
 * @brief Dispatches execution to the appropriate builtin function.
 * @details Identifies the command name and calls the corresponding builtin 
 * handler function.
 * - Special Case: For 'exit', it checks if the session is interactive 
 * (isatty) and prints "exit" to STDERR before calling the handler, ensuring 
 * Bash-like behavior.
 * @param mini Pointer to the main minishell structure.
 * @param cmd  Pointer to the command structure containing arguments.
 * @return     The exit status returned by the specific builtin function.
 */
int	exec_builtin(t_mini *mini, t_cmd *cmd)
{
	if (!ft_strcmp("echo", cmd->argv[0]))
		return (builtin_echo(mini, cmd->argv));
	if (!ft_strcmp("cd", cmd->argv[0]))
		return (builtin_cd(mini, cmd->argv));
	if (!ft_strcmp("pwd", cmd->argv[0]))
		return (builtin_pwd(mini, cmd->argv));
	if (!ft_strcmp("export", cmd->argv[0]))
		return (builtin_export(mini, cmd->argv));
	if (!ft_strcmp("unset", cmd->argv[0]))
		return (builtin_unset(mini, cmd->argv));
	if (!ft_strcmp("env", cmd->argv[0]))
		return (builtin_env(mini, cmd->argv));
	if (!ft_strcmp("exit", cmd->argv[0]))
		return (builtin_exit(mini, cmd->argv));
	return (EXIT_FAILURE);
}
