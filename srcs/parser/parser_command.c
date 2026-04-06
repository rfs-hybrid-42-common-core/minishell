/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 20:12:32 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/01/22 23:06:15 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/**
 * @fn bool is_cmd_token(t_token *token)
 * @brief Checks if a token belongs to a command body.
 * @details Commands consist of Words and Redirections (including quoted
 * heredocs).
 * @param token The token to check.
 * @return      true if valid command part, false otherwise (e.g., pipe, logic).
 */
bool	is_cmd_token(t_token *token)
{
	if (token->type == TOKEN_WORD || token->type == TOKEN_APPEND
		|| token->type == TOKEN_HEREDOC || token->type == TOKEN_HEREDOC_QUOTED
		|| token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT)
		return (true);
	return (false);
}

/**
 * @fn static bool cmd_add_arg(t_cmd *cmd, char *word)
 * @brief Adds an argument to the command's argv array.
 * @details Reallocates the array to size+1 and appends the new word.
 * @param cmd  Pointer to the command structure.
 * @param word The argument string to add.
 * @return     true on success, false on allocation error.
 */
static bool	cmd_add_arg(t_cmd *cmd, char *word)
{
	char	**new;
	int		i;

	i = 0;
	if (cmd->argv)
		while (cmd->argv[i])
			i++;
	new = ft_calloc(i + 2, sizeof(char *));
	if (!new)
		return (false);
	i = 0;
	if (cmd->argv)
	{
		while (cmd->argv[i])
		{
			new[i] = cmd->argv[i];
			i++;
		}
	}
	new[i] = ft_strdup(word);
	if (!new[i])
		return (free(new), false);
	free(cmd->argv);
	cmd->argv = new;
	return (true);
}

/**
 * @fn bool cmd_add_redir(t_cmd *cmd, t_token_type type, char *file)
 * @brief Adds a redirection to the command's redirection list.
 * @param cmd  Pointer to the command structure.
 * @param type The type of redirection.
 * @param file The filename or delimiter.
 * @return     true on success, false on error.
 */
bool	cmd_add_redir(t_cmd *cmd, t_token_type type, char *file)
{
	t_redir	*new;
	t_redir	*temp;

	new = new_redir(type, file);
	if (!new)
		return (false);
	if (!cmd->redirs)
	{
		cmd->redirs = new;
		return (true);
	}
	temp = cmd->redirs;
	while (temp->next)
		temp = temp->next;
	temp->next = new;
	return (true);
}

/**
 * @fn static t_cmd *parse_cmd_body(t_token **tokens)
 * @brief Assembles the arguments and redirections of a simple command.
 * @details Iterates through tokens, adding words to the argv list and
 * redirections to the redirs list.
 * @param tokens Double pointer to current token list.
 * @return       Pointer to the populated command structure, or NULL on error.
 */
static t_cmd	*parse_cmd_body(t_token **tokens)
{
	t_cmd			*cmd;
	t_token_type	type;

	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	while (*tokens && is_cmd_token(*tokens))
	{
		if ((*tokens)->type == TOKEN_WORD && !cmd_add_arg(cmd, (*tokens)->val))
			return (free_cmd(cmd), NULL);
		else if ((*tokens)->type != TOKEN_WORD)
		{
			type = (*tokens)->type;
			*tokens = (*tokens)->next;
			if (!*tokens || (*tokens)->type != TOKEN_WORD)
				return (print_end_error(*tokens), free_cmd(cmd), NULL);
			if (!cmd_add_redir(cmd, type, (*tokens)->val))
				return (free_cmd(cmd), NULL);
		}
		*tokens = (*tokens)->next;
	}
	return (cmd);
}

/**
 * @fn t_ast *parse_single_command(t_token **tokens)
 * @brief Entry point for command level parsing.
 * @details Dispatches to parse_subshell (if starts with '(') or
 * parse_cmd_body for regular commands.
 * - Handles redirections immediately following subshells (e.g., "(...) > out").
 * @param tokens Double pointer to current token list.
 * @return       Pointer to the AST node (AST_CMD or AST_SUBSHELL).
 */
t_ast	*parse_single_command(t_token **tokens)
{
	t_ast	*node;
	t_cmd	*cmd;

	if (!*tokens)
		return (NULL);
	if ((*tokens)->type == TOKEN_LPAREN)
	{
		node = parse_subshell(tokens);
		if (!node)
			return (NULL);
		if (!parse_subshell_redirs(node, tokens))
			return (free_ast(node), NULL);
		return (node);
	}
	node = new_tree_node(AST_CMD);
	if (!node)
		return (NULL);
	cmd = parse_cmd_body(tokens);
	if (!cmd)
		return (free(node), NULL);
	if (!cmd->argv && !cmd->redirs)
		return (free_cmd(cmd), free(node), NULL);
	node->cmd = cmd;
	return (node);
}
