/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 22:52:21 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:24:02 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "builtin.h"

/**
 * @fn static char *find_path_in_env(char *cmd, char **paths)
 * @brief Helper to search for a binary in a list of paths.
 * @details Iterates through the provided paths array (derived from $PATH). 
 * For each directory, it joins "dir" + "/" + "cmd" and checks if the file 
 * exists.
 * @param cmd   The command name to look for (e.g., "grep").
 * @param paths A NULL-terminated array of directory strings.
 * @return      The allocated full path if found, or NULL if not found.
 */
static char	*find_path_in_env(char *cmd, char **paths)
{
	char	*full_path;
	char	*partial_path;
	int		i;

	i = -1;
	while (paths && paths[++i])
	{
		partial_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(partial_path, cmd);
		free(partial_path);
		if (!access(full_path, F_OK))
			return (full_path);
		free(full_path);
	}
	return (NULL);
}

/**
 * @fn static char *get_path(t_mini *mini, char *cmd)
 * @brief Resolves the full path of an executable command.
 * @details Checks the PATH environment variable, splits it, and checks for the 
 * existence and executability of the command. If the command already contains 
 * a slash, it is returned as-is.
 * @param mini Pointer to the main minishell structure.
 * @param cmd  The command name to resolve (e.g., "ls").
 * @return     Allocated string containing full path, or NULL if not found.
 */
static char	*get_path(t_mini *mini, char *cmd)
{
	t_env	*env;
	char	**paths;
	char	*env_path;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	env = find_env(mini->env, "PATH");
	if (!env || !env->val)
		return (NULL);
	paths = ft_split(env->val, ':');
	env_path = find_path_in_env(cmd, paths);
	free_matrix(paths);
	return (env_path);
}

/**
 * @fn static void validate_cmd_path(t_mini *mini, char *path, char *cmd)
 * @brief Validates the command path and handles specific execution errors.
 * @details Checks for existence, directory status, and execution permissions. 
 * If any check fails, it prints the appropriate error message, frees all 
 * resources (child process cleanup) and exits with:
 * - 127: Command not found or No such file or directory.
 * - 126: Is a directory or Permission denied.
 * @param mini Pointer to the main minishell struct (for cleanup).
 * @param path The resolved path to the command binary.
 * @param cmd  The original command name (for error messages).
 */
static void	validate_cmd_path(t_mini *mini, char *path, char *cmd)
{
	if (!path || access(path, F_OK) == -1)
	{
		if (!path)
			print_command_error(cmd, "command not found");
		else
			print_command_error(cmd, "No such file or directory");
		free(path);
		cleanup_iteration(mini);
		free_mini(mini);
		exit(EXIT_NOT_FOUND);
	}
	if (is_directory(path) || access(path, X_OK) == -1)
	{
		if (is_directory(path))
			print_command_error(cmd, "Is a directory");
		else
			print_command_error(cmd, "Permission denied");
		free(path);
		cleanup_iteration(mini);
		free_mini(mini);
		exit(EXIT_NO_EXEC);
	}
}

/**
 * @fn static void exec_command_child(t_mini *mini, t_cmd *cmd)
 * @brief Logic for the child process of an external command.
 * @details 
 * 1. Resolves the binary path using the PATH variable.
 * 2. Validates the path (exits and frees if invalid).
 * 3. Converts environment, executes command.
 * 4. If execve fails, frees everything and exits.
 * @param mini Pointer to the main minishell structure.
 * @param cmd  Pointer to the command structure containing args.
 */
static void	exec_command_child(t_mini *mini, t_cmd *cmd)
{
	char	*path;
	char	**envp;

	path = get_path(mini, cmd->argv[0]);
	validate_cmd_path(mini, path, cmd->argv[0]);
	update_env_var(mini, "_", path);
	envp = env_list_to_array(mini->env);
	execve(path, cmd->argv, envp);
	print_command_error(cmd->argv[0], strerror(errno));
	free_matrix(envp);
	free(path);
	cleanup_iteration(mini);
	free_mini(mini);
	exit(EXIT_NO_EXEC);
}

/**
 * @fn int exec_external(t_mini *mini, t_cmd *cmd, t_io *io)
 * @brief Prepares and executes an external command.
 * @details Forks a new process:
 * - **Child**:
 * - Closes the inherited backup file descriptors (`io->in`, `io->out`) 
 * to prevent FD leaks if execution fails or exits early.
 * - Resets signal handlers and calls `exec_command_child`.
 * - **Parent**:
 * - Ignores SIGINT (waits for child).
 * - Blocks until the child process finishes using `waitpid`.
 * @param mini Pointer to the main minishell structure.
 * @param cmd  Pointer to the command structure.
 * @param io   Pointer to the IO structure containing backup FDs to close.
 * @return     The exit status of the command (converted from waitpid status).
 */
int	exec_external(t_mini *mini, t_cmd *cmd, t_io *io)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork");
		return (EXIT_FAILURE);
	}
	if (!pid)
	{
		if (io)
		{
			if (io->in != -1)
				close(io->in);
			if (io->out != -1)
				close(io->out);
		}
		setup_child_signals();
		exec_command_child(mini, cmd);
	}
	setup_parent_signals();
	waitpid(pid, &status, 0);
	setup_signals();
	return (exec_exit_status(status));
}
