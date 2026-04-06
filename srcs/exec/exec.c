/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 19:17:14 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:29:07 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn int exec(t_mini *mini, t_ast *ast)
 * @brief Dispatcher for the AST execution.
 * @details Identifies the type of the current AST node (Command, Pipe, Logic, 
 * Subshell) and calls the appropriate handler function.
 * @param mini Pointer to the main minishell structure.
 * @param ast  Pointer to the current AST node to execute.
 * @return     The exit status of the executed node.
 */
int	exec(t_mini *mini, t_ast *ast)
{
	if (!ast)
		return (mini->exit_code);
	if (ast->type == AST_CMD)
		return (exec_command(mini, ast->cmd));
	if (ast->type == AST_PIPE)
		return (exec_pipeline(mini, ast->lnode, ast->rnode));
	if (ast->type == AST_AND || ast->type == AST_OR)
		return (exec_logic(mini, ast));
	if (ast->type == AST_SUBSHELL)
		return (exec_subshell(mini, ast));
	return (EXIT_FAILURE);
}
