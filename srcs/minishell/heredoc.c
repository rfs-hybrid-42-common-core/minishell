/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 01:50:27 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:46:56 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "exec.h"

/**
 * @fn static char *generate_heredoc_file(void)
 * @brief Generates a unique filename for a temporary heredoc file.
 * @details Uses a static counter to create a unique path (e.g., 
 * "/tmp/.here_doc_0"). To prevents collisions with other running shell 
 * instances, it loops and increments the index until it finds a filename 
 * that does not currently exist on disk (checked via access).
 * @return Allocated string containing the unique temporary filename.
 */
static char	*generate_heredoc_file(void)
{
	static int	i = 0;
	char		*num;
	char		*file;

	while (true)
	{
		num = ft_itoa(i++);
		file = ft_strjoin("/tmp/.here_doc_", num);
		free(num);
		if (access(file, F_OK) == -1)
			return (file);
		free(file);
	}
}

/**
 * @fn static bool handle_cmd_heredoc(t_mini *mini, t_redir *redir)
 * @brief Processes all heredocs associated with a single command.
 * @details Iterates through redirs. For HEREDOC or HEREDOC_QUOTED:
 * 1. Determines expansion mode (enabled only for TOKEN_HEREDOC).
 * 2. Generates a temp filename.
 * 3. Opens the file and calls `exec_heredoc` to fill it via child process.
 * 4. Updates the redir node with the path to the temp file.
 * @param mini  Pointer to the main shell structure.
 * @param redir Pointer to the head of the redirection list.
 * @return      true on success, false if interrupted (SIGINT).
 */
static bool	handle_cmd_heredoc(t_mini *mini, t_redir *redir)
{
	char	*lim;
	bool	expand;
	int		fd;
	int		status;

	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC || redir->type == TOKEN_HEREDOC_QUOTED)
		{
			lim = redir->file;
			expand = (redir->type == TOKEN_HEREDOC);
			redir->file = generate_heredoc_file();
			fd = open(redir->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
			if (fd == -1)
				return (free(lim), false);
			status = exec_heredoc(mini, fd, lim, expand);
			close(fd);
			free(lim);
			if (status == EXIT_FAILURE)
				return (false);
		}
		redir = redir->next;
	}
	return (true);
}

/**
 * @fn bool process_heredoc(t_mini *mini, t_ast *ast)
 * @brief Pre-processes the AST to handle all Here-Documents.
 * @details Recursively scans the AST. For every heredoc token found, generates 
 * a temp file and prompts the user for input before execution starts.
 * @param mini Pointer to the main minishell structure.
 * @param node Root of the AST to scan.
 * @return     true if successful, false if interrupted.
 */
bool	process_heredoc(t_mini *mini, t_ast *ast)
{
	if (!ast)
		return (true);
	if (ast->type == AST_PIPE || ast->type == AST_AND || ast->type == AST_OR)
	{
		if (!process_heredoc(mini, ast->lnode))
			return (false);
		return (process_heredoc(mini, ast->rnode));
	}
	if (ast->type == AST_SUBSHELL)
		return (process_heredoc(mini, ast->lnode));
	if (ast->type == AST_CMD)
		return (handle_cmd_heredoc(mini, ast->cmd->redirs));
	return (true);
}
