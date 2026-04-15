/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 12:24:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/15 22:12:13 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

/**
 * @fn static void init_env(t_mini *mini, char **envp)
 * @brief Initializes the shell's environment list.
 * @details Copies the environment variables from the `envp` array into a 
 * linked list. Each node key and value is duplicated.
 * @param mini Pointer to the main shell structure.
 * @param envp The environment variables array from main.
 */
static void	init_env(t_mini *mini, char **envp)
{
	t_env	*node;
	char	*eq;
	int		i;

	mini->env = NULL;
	if (!envp)
		return ;
	i = -1;
	while (envp[++i])
	{
		eq = ft_strchr(envp[i], '=');
		if (!eq)
			continue ;
		node = malloc(sizeof(t_env));
		if (!node)
			exit(EXIT_FAILURE);
		node->key = ft_substr(envp[i], 0, eq - envp[i]);
		node->val = ft_strdup(eq + 1);
		node->exported = true;
		node->next = NULL;
		env_add_back(&mini->env, node);
	}
	if (!find_env(mini->env, "PATH"))
		env_add_back(&mini->env, env_new("PATH", \
"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", true));
}

/**
 * @fn static void init_pid(t_mini *mini)
 * @brief Initializes the pid_str variable.
 * @details Reads the shell's PID from /proc/self/stat and stores it as a 
 * string in the main struct. This is used for `$$` expansion.
 * Falls back to "0" if reading fails.
 * @param mini Pointer to the main shell structure.
 */
static void	init_pid(t_mini *mini)
{
	char	buffer[PID_SIZE];
	int		fd;
	int		bytes;
	int		i;

	fd = open("/proc/self/stat", O_RDONLY);
	if (fd == -1)
	{
		mini->pid_str = ft_strdup("0");
		return ;
	}
	bytes = read(fd, buffer, PID_SIZE - 1);
	close(fd);
	if (bytes <= 0)
	{
		mini->pid_str = ft_strdup("0");
		return ;
	}
	buffer[bytes] = '\0';
	i = 0;
	while (buffer[i] && ft_isdigit(buffer[i]))
		i++;
	mini->pid_str = ft_substr(buffer, 0, i);
}

/**
 * @fn static int get_shlvl_value(char *val)
 * @brief Parses the current SHLVL string strictly.
 * @details Checks if the string is numeric.
 * - Skips leading sign (+/-).
 * - If non-numeric chars are found, returns 0 (triggers reset to 1).
 * - Otherwise calls ft_atoi.
 * @param val The string value of SHLVL.
 * @return    The integer value, or 0 if invalid.
 */
static int	get_current_shlvl(char *val)
{
	int	i;

	i = 0;
	if (val[i] == '-' || val[i] == '+')
		i++;
	while (val[i])
	{
		if (!ft_isdigit(val[i]))
			return (0);
		i++;
	}
	return (ft_atoi(val));
}

/**
 * @fn static void update_shlvl(t_mini *mini)
 * @brief Updates the SHLVL environment variable.
 * @details 
 * 1. Reads current level (defaults to 1 if unset/invalid).
 * 2. Increments the level.
 * 3. Checks limits:
 * - If > 1000: Prints warning and resets to 1.
 * - If < 0: Resets to 0.
 * 4. Updates or creates the env variable.
 * @param mini Pointer to the main shell structure.
 */
static void	update_shlvl(t_mini *mini)
{
	t_env	*env;
	char	*val;
	int		shlvl;

	env = find_env(mini->env, "SHLVL");
	shlvl = 1;
	if (env && env->val)
		shlvl = get_current_shlvl(env->val) + 1;
	if (shlvl >= 1000)
	{
		ft_putstr_fd("minishell: warning: shell level (", STDERR_FILENO);
		ft_putnbr_fd(shlvl, STDERR_FILENO);
		ft_putendl_fd(") too high, resetting to 1", STDERR_FILENO);
		shlvl = 1;
	}
	if (shlvl < 0)
		shlvl = 0;
	val = ft_itoa(shlvl);
	if (env)
	{
		free(env->val);
		env->val = val;
		return ;
	}
	return (env_add_back(&mini->env, env_new("SHLVL", val, true)), free(val));
}

/**
 * @fn void init_minishell(t_mini *mini, char **envp)
 * @brief Initializes the main shell structure.
 * @details Sets initial values for pointers, copies environment variables, 
 * updates the shell level, ensures PWD exists (handling empty environments), 
 * and configures signal handlers.
 * @param mini Pointer to the main shell structure.
 * @param envp The environment variables array.
 */
void	init_minishell(t_mini *mini, char **envp)
{
	char	*cwd;

	mini->prompt.login = NULL;
	mini->prompt.line = NULL;
	mini->tokens = NULL;
	mini->ast = NULL;
	mini->exit_code = EXIT_SUCCESS;
	mini->line_count = 0;
	init_env(mini, envp);
	init_pid(mini);
	update_shlvl(mini);
	if (!find_env(mini->env, "PWD"))
	{
		cwd = getcwd(NULL, 0);
		if (cwd)
		{
			env_add_back(&mini->env, env_new("PWD", cwd, true));
			free(cwd);
		}
	}
	setup_signals();
}
