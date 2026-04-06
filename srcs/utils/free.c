/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:46:41 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:56:14 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/**
 * @fn void free_matrix(char **arr)
 * @brief Frees a dynamically allocated 2D array of strings (char **).
 * @details Iterates through the array, freeing each individual string, and 
 * finally frees the array pointer itself. Safe to call with NULL.
 * @param arr The null-terminated array of strings to free.
 */
void	free_matrix(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = -1;
	while (arr[++i])
		free(arr[i]);
	free(arr);
}

/**
 * @fn void free_cmd(t_cmd *cmd)
 * @brief Frees a command structure and its contents.
 * @details Frees the argument array (argv), the linked list of redirections, 
 * and finally the command structure itself.
 * @param cmd Pointer to the command structure to free.
 */
void	free_cmd(t_cmd *cmd)
{
	t_redir	*redir;
	t_redir	*next;

	if (!cmd)
		return ;
	free_matrix(cmd->argv);
	redir = cmd->redirs;
	while (redir)
	{
		next = redir->next;
		free(redir->file);
		free(redir);
		redir = next;
	}
	free(cmd);
}

/**
 * @fn void free_ast(t_ast *ast)
 * @brief Recursively frees the Abstract Syntax Tree (AST).
 * @details Traverses the AST in post-order, freeing left/right children, 
 * the command data (if present), and the node itself.
 * @param ast Root of the AST subtree to free.
 */
void	free_ast(t_ast *ast)
{
	if (!ast)
		return ;
	free_ast(ast->lnode);
	free_ast(ast->rnode);
	free_cmd(ast->cmd);
	free(ast);
}

/**
 * @fn void free_tokens(t_token **tokens)
 * @brief Frees the token linked list.
 * @details Iterates through the token list, freeing the value string and 
 * the node. Sets the head pointer to NULL after completion.
 * @param tokens Double pointer to the head of the token list.
 */
void	free_tokens(t_token **tokens)
{
	t_token	*curr;
	t_token	*next;

	if (!tokens || !*tokens)
		return ;
	curr = *tokens;
	while (curr)
	{
		next = curr->next;
		free(curr->val);
		free(curr);
		curr = next;
	}
	*tokens = NULL;
}

/**
 * @fn void free_mini(t_mini *mini)
 * @brief Performs a full cleanup of the shell structure upon exit.
 * @details Frees all dynamic memory associated with the shell session, 
 * including environment variables, history, and the persistent pid_str.
 * @param mini Pointer to the main shell structure.
 */
void	free_mini(t_mini *mini)
{
	cleanup_iteration(mini);
	env_list_clear(&mini->env);
	if (mini->prompt.login)
		free(mini->prompt.login);
	if (mini->pid_str)
		free(mini->pid_str);
	rl_clear_history();
}
