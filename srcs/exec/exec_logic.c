/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:58:37 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:26:50 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn int exec_logic(t_mini *mini, t_ast *ast)
 * @brief Handles logical operators '&&' and '||'.
 * @details Executes the left node first. Depending on the exit status and the 
 * operator type, decides whether to execute the right node (short-circuiting).
 * @param mini Pointer to the main minishell structure.
 * @param ast  Pointer to the AST node containing the logic operator.
 * @return     The exit status of the last executed command.
 */
int	exec_logic(t_mini *mini, t_ast *ast)
{
	int	exit_code;

	exit_code = exec(mini, ast->lnode);
	mini->exit_code = exit_code;
	if ((ast->type == AST_AND && exit_code == 0)
		|| (ast->type == AST_OR && exit_code != 0))
	{
		exit_code = exec(mini, ast->rnode);
		mini->exit_code = exit_code;
	}
	return (exit_code);
}
