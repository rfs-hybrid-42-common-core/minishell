/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:00:30 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:04:50 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "expand.h"

/**
 * @fn char *get_raw_input(void)
 * @brief Retrieves a single line of input for the heredoc.
 * @details 
 * Abstracts the input method based on the shell's state:
 * - **Interactive (isatty)**: Uses `readline("> ")` to prompt the user.
 * - **Non-Interactive**: Uses `get_next_line(STDIN)` to read from the pipe 
 * or file. Manually removes the trailing newline from GNL's output to 
 * match `readline`'s behavior (allowing consistent delimiter checks).
 * @return A newly allocated string containing the line (without newline), 
 * or NULL on EOF.
 */
char	*get_raw_input(void)
{
	char	*line;
	size_t	len;

	if (isatty(STDIN_FILENO))
		return (readline("> "));
	line = get_next_line(STDIN_FILENO);
	if (line)
	{
		len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
	}
	return (line);
}

/**
 * @fn void print_heredoc(t_mini *mini, int fd, char *line, bool expand)
 * @brief Writes a processed line to the heredoc temporary file.
 * @details Handles the logic for variable expansion within a heredoc:
 * - If `expand` is TRUE (the delimiter was NOT quoted) AND the line contains 
 * a '$' character, it calls `expand_heredoc` to substitute variables.
 * - Otherwise, it writes the raw line to the file descriptor `fd`.
 * @param mini   Pointer to the main shell structure.
 * @param fd     The file descriptor of the temporary heredoc file.
 * @param line   The line read from input.
 * @param expand Boolean flag indicating if variable expansion is required.
 */
void	print_heredoc(t_mini *mini, int fd, char *line, bool expand)
{
	char	*expanded;

	if (expand && ft_strchr(line, '$'))
	{
		expanded = expand_heredoc(mini, line);
		ft_putendl_fd(expanded, fd);
		free(expanded);
	}
	else
		ft_putendl_fd(line, fd);
}

/**
 * @fn void print_heredoc_warning(int line_count, char *lim)
 * @brief Prints a warning when a heredoc is terminated by EOF.
 * @details 
 * Displays the Bash-compliant warning message:
 * `minishell: warning: here-document at line X delimited by end-of-file 
 * (wanted 'LIM')`
 * This occurs when the user presses Ctrl+D before typing the delimiter. 
 * The warning is printed to STDERR (fd 2).
 * @param line_count The current line number where the heredoc started.
 * @param lim        The expected delimiter string.
 */
void	print_heredoc_warning(int line_count, char *lim)
{
	ft_putstr_fd("minishell: warning: here-document at line ", STDERR_FILENO);
	ft_putnbr_fd(line_count, STDERR_FILENO);
	ft_putstr_fd(" delimited by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(lim, STDERR_FILENO);
	ft_putendl_fd("')", STDERR_FILENO);
}
