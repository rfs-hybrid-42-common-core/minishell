/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:00:55 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:16:28 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn static char *recover_path(char *path, char *old_pwd)
 * @brief Logic fallback when getcwd fails (e.g., directory was deleted).
 * @details
 * This function is called when `getcwd` returns NULL after a successful 
 * `chdir`.
 * It prints the standard "error retrieving current directory" warning and 
 * attempts to logically reconstruct the current path:
 * - If `path` is absolute, it returns `path`.
 * - If `path` is relative, it joins `old_pwd` + `/` + `path`.
 * @param path    The path argument passed to cd.
 * @param old_pwd The directory we were in before `chdir` was called.
 * @return        A newly allocated string representing the logical PWD.
 */
static char	*recover_path(char *path, char *old_pwd)
{
	char	*tmp;
	char	*res;

	ft_putendl_fd("cd: error retrieving current directory: getcwd: "
		"cannot access parent directories: No such file or directory",
		STDERR_FILENO);
	if (path[0] == '/')
		return (ft_strdup(path));
	tmp = ft_strjoin(old_pwd, "/");
	res = ft_strjoin(tmp, path);
	free(tmp);
	return (res);
}

/**
 * @fn static int cd_change_dir(t_mini *mini, char *path, char *old_pwd)
 * @brief Performs the directory change and updates environment variables.
 * @details Executes the chdir system call.
 * 1. If chdir fails, prints the system error (e.g., "Permission denied").
 * 2. If successful, attempts to get the new absolute path via `getcwd`.
 * 3. If `getcwd` fails (e.g., directory deleted), calls `recover_path` 
 * to reconstruct the path logically.
 * 4. Updates OLDPWD and PWD in the environment.
 * @param mini    Pointer to the main shell structure.
 * @param path    The target directory path.
 * @param old_pwd The saved path of the previous directory.
 * @return        EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
static int	cd_change_dir(t_mini *mini, char *path, char *old_pwd)
{
	char	*new_pwd;

	if (chdir(path) < 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(strerror(errno), STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		new_pwd = recover_path(path, old_pwd);
	update_env_var(mini, "OLDPWD", old_pwd);
	update_env_var(mini, "PWD", new_pwd);
	free(new_pwd);
	return (EXIT_SUCCESS);
}

/**
 * @fn static int cd_no_args(t_mini *mini)
 * @brief Handles the 'cd' command with no arguments (Go to HOME).
 * @details Retrieves the HOME variable. If set, it changes the directory to 
 * that path. If HOME is unset, prints "HOME not set".
 * @param mini Pointer to the main shell structure.
 * @return     EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
static int	cd_no_args(t_mini *mini)
{
	t_env	*home;
	char	*old_pwd;
	int		status;

	home = find_env(mini->env, "HOME");
	if (!home || !home->val)
	{
		print_command_error("cd", "HOME not set");
		return (EXIT_FAILURE);
	}
	old_pwd = get_old_pwd(mini);
	status = cd_change_dir(mini, home->val, old_pwd);
	free(old_pwd);
	return (status);
}

/**
 * @fn static int cd_dash(t_mini *mini)
 * @brief Handles the 'cd -' command (Go to previous directory).
 * @details Retrieves the OLDPWD variable. If set, changes to that directory. 
 * Prints the new path to stdout only upon success.
 * @param mini Pointer to the main shell structure.
 * @return     EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
static int	cd_dash(t_mini *mini)
{
	t_env	*old_env;
	char	*old_pwd;
	char	*target;
	int		status;

	old_env = find_env(mini->env, "OLDPWD");
	if (!old_env || !old_env->val)
	{
		print_command_error("cd", "OLDPWD not set");
		return (EXIT_FAILURE);
	}
	target = ft_strdup(old_env->val);
	old_pwd = get_old_pwd(mini);
	status = cd_change_dir(mini, target, old_pwd);
	if (status == EXIT_SUCCESS)
		ft_putendl_fd(target, STDOUT_FILENO);
	free(target);
	free(old_pwd);
	return (status);
}

/**
 * @fn int builtin_cd(t_mini *mini, char **argv)
 * @brief Main entry point for the cd builtin.
 * @details Dispatches execution based on arguments and flags:
 * - Checks for the "--" flag (End of Options). If present, shifts the target 
 * argument index from 1 to 2.
 * - **No args** (or only "--"): Calls `cd_no_args` (Go to HOME).
 * - **"-"**: Calls `cd_dash` (Go to OLDPWD), but only if it's the first 
 * argument (e.g., `cd -` works, but `cd -- -` goes to a folder named "-").
 * - **Path**: Calls `cd_change_dir` to go to the specified path.
 * - Handles "too many arguments" errors.
 * @param mini Pointer to the main shell structure.
 * @param argv Null-terminated array of arguments (argv[0] is "cd").
 * @return     EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int	builtin_cd(t_mini *mini, char **argv)
{
	char	*old_pwd;
	int		status;
	int		i;

	i = 1;
	if (argv[1] && !ft_strcmp(argv[1], "--"))
		i = 2;
	if (argv[i] && argv[i + 1])
		return (print_command_error("cd", "too many arguments"), EXIT_FAILURE);
	if (!argv[i])
		return (cd_no_args(mini));
	if (!*argv[i])
		return (EXIT_SUCCESS);
	if (!ft_strcmp(argv[i], "-") && i == 1)
		return (cd_dash(mini));
	old_pwd = get_old_pwd(mini);
	status = cd_change_dir(mini, argv[i], old_pwd);
	free(old_pwd);
	return (status);
}
