/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 14:01:37 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:36:07 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn static void exec_heredoc_parent(t_mini *mini, int fd, char *lim, 
 * bool expand)
 * @brief Handles heredoc reading in the parent process (non-interactive mode).
 * @details This function is used when `isatty` is false (e.g., during 
 * automated testing or piped input). It reads directly from STDIN using 
 * `get_next_line` to ensure the shell shares the same input buffer as the 
 * main process.
 * 1. Reads line-by-line using GNL.
 * 2. Strips the newline character to match the delimiter check.
 * 3. Increments the global line count.
 * 4. Breaks if the line matches `lim` or if EOF is reached.
 * 5. Writes the line (expanded or raw) to the temporary file `fd`.
 * @param mini   Pointer to the main shell structure.
 * @param fd     File descriptor of the temporary file to write to.
 * @param lim    The delimiter string (without newline).
 * @param expand Boolean flag for variable expansion.
 */
static void	exec_heredoc_parent(t_mini *mini, int fd, char *lim, bool expand)
{
	char	*line;
	size_t	len;

	while (true)
	{
		line = get_next_line(STDIN_FILENO);
		if (line)
		{
			len = ft_strlen(line);
			if (len > 0 && line[len - 1] == '\n')
				line[len - 1] = '\0';
			mini->line_count++;
		}
		else
			break ;
		if (!ft_strcmp(line, lim))
		{
			free(line);
			break ;
		}
		print_heredoc(mini, fd, line, expand);
		free(line);
	}
}

/**
 * @fn static void heredoc_readline(t_mini *mini, int *fds, char *lim, 
 * bool expand)
 * @brief Main input loop for the heredoc child process (interactive mode).
 * @details Executed inside a child process. It uses `readline` to prompt the 
 * user.
 * 1. **Input**: Calls `get_raw_input` (wraps readline/GNL logic).
 * 2. **Line Counting**: Writes `1` to the pipe `fds[1]` for every line read. 
 * This keeps the parent's line count synced.
 * 3. **EOF Handling**: Prints a warning if `line` is NULL (Ctrl+D).
 * 4. **Delimiter**: Breaks the loop if `line` matches `lim`.
 * 5. **Output**: Calls `print_heredoc` to write to the temp file (`fds[0]`).
 * 6. **Cleanup**: Frees memory and exits the child process.
 * @param mini   Pointer to the main shell structure.
 * @param fds    Array where fds[0] is the TEMP FILE FD and fds[1] is the 
 * COUNTING PIPE WRITE END.
 * @param lim    The delimiter string.
 * @param expand Boolean flag for variable expansion.
 */
static void	heredoc_readline(t_mini *mini, int *fds, char *lim, bool expand)
{
	char	*line;
	int		one;

	one = 1;
	while (true)
	{
		line = get_raw_input();
		if (line)
			write(fds[1], &one, sizeof(int));
		else
		{
			if (isatty(STDIN_FILENO))
				print_heredoc_warning(mini->line_count, lim);
			break ;
		}
		if (!ft_strcmp(line, lim))
			break ;
		print_heredoc(mini, fds[0], line, expand);
		free(line);
	}
	close(fds[0]);
	close(fds[1]);
	return (free(line), free(lim), cleanup_iteration(mini), free_mini(mini));
}

/**
 * @fn static int wait_heredoc_execution(t_mini *mini, pid_t pid, int fd)
 * @brief Waits for the child process and synchronizes the line count.
 * @details This function blocks until the heredoc child process finishes.
 * 1. **Wait**: Uses `waitpid` to get the child's exit status.
 * 2. **Sync**: Reads integers from the pipe `fd` and increments 
 * `mini->line_count` accordingly.
 * 3. **Signal Handling**: If the child was terminated by SIGINT (Ctrl+C),
 * it prints a newline, updates `g_signal`, and returns failure.
 * @param mini Pointer to the main shell structure (updates line_count).
 * @param pid  The process ID of the heredoc child.
 * @param fd   The read-end of the counting pipe.
 * @return     EXIT_SUCCESS or EXIT_FAILURE (if SIGINT occurred).
 */
static int	wait_heredoc_execution(t_mini *mini, pid_t pid, int fd)
{
	int	status;
	int	count_buff;

	waitpid(pid, &status, 0);
	count_buff = 0;
	while (read(fd, &count_buff, sizeof(int)) > 0)
		mini->line_count += count_buff;
	close(fd);
	setup_signals();
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		g_signal = SIGINT;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * @fn int exec_heredoc(t_mini *mini, int fd, char *lim, bool expand)
 * @brief Orchestrates the Here-Document execution.
 * @details 
 * Determines the execution mode based on whether the input is a TTY.
 * **Non-Interactive (!isatty)**:
 * - Calls `exec_heredoc_parent` to read directly in the main process.
 * - This prevents buffer loss when reading from pipes/testers.
 * **Interactive (isatty)**:
 * - Creates a pipe `p_fd` for line counting.
 * - Forks a child process.
 * - **Child**: Runs `heredoc_readline`.
 * - **Parent**: Runs `wait_heredoc_execution`.
 * @param mini   Main shell structure.
 * @param fd     File descriptor of the temporary file.
 * @param lim    Delimiter string.
 * @param expand Expansion flag.
 * @return       EXIT_SUCCESS or EXIT_FAILURE.
 */
int	exec_heredoc(t_mini *mini, int fd, char *lim, bool expand)
{
	pid_t	pid;
	int		p_fd[2];

	if (!isatty(STDIN_FILENO))
		return (exec_heredoc_parent(mini, fd, lim, expand), EXIT_SUCCESS);
	if (pipe(p_fd) == -1)
		return (perror("pipe"), EXIT_FAILURE);
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		close(p_fd[0]);
		close(p_fd[1]);
		return (perror("fork"), EXIT_FAILURE);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		close(p_fd[0]);
		p_fd[0] = fd;
		heredoc_readline(mini, p_fd, lim, expand);
		exit(EXIT_SUCCESS);
	}
	close(p_fd[1]);
	return (wait_heredoc_execution(mini, pid, p_fd[0]));
}
