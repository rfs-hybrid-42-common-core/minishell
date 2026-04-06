/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 15:34:33 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:46:21 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @fn void cleanup_iteration(t_mini *mini)
 * @brief Frees resources used in a single shell loop iteration.
 * @details Safely frees the AST, tokens, and the command line string 
 * to prepare for the next prompt.
 * @param mini Pointer to the main shell structure.
 */
void	cleanup_iteration(t_mini *mini)
{
	if (mini->ast)
	{
		free_ast(mini->ast);
		mini->ast = NULL;
	}
	if (mini->tokens)
	{
		free_tokens(&mini->tokens);
		mini->tokens = NULL;
	}
	if (mini->prompt.line)
	{
		free(mini->prompt.line);
		mini->prompt.line = NULL;
	}
}

/**
 * @fn void cleanup_heredoc(t_ast *ast)
 * @brief Cleans up temporary Here-Document files.
 * @details Traverses the AST after execution and unlinks (deletes) 
 * any temporary files created during the heredoc phase.
 * @param node Root of the AST to scan.
 */
void	cleanup_heredoc(t_ast *ast)
{
	t_redir	*redir;

	if (!ast)
		return ;
	if (ast->type == AST_PIPE || ast->type == AST_AND || ast->type == AST_OR)
	{
		cleanup_heredoc(ast->lnode);
		cleanup_heredoc(ast->rnode);
	}
	if (ast->type == AST_SUBSHELL)
		cleanup_heredoc(ast->lnode);
	if (ast->type == AST_CMD && ast->cmd)
	{
		redir = ast->cmd->redirs;
		while (redir)
		{
			if ((redir->type == TOKEN_HEREDOC
					|| redir->type == TOKEN_HEREDOC_QUOTED) && redir->file)
				unlink(redir->file);
			redir = redir->next;
		}
	}
}
