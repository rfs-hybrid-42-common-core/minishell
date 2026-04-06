/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:02:32 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:19:49 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn void update_env_var(t_mini *mini, char *key, char *value)
 * @brief Updates or creates an environment variable.
 * @details Helper function that searches for a key in the environment list. 
 * If found, it frees the old value and assigns the new one. If not found, 
 * it creates a new node and appends it to the list.
 * @param mini  Pointer to the main shell structure containing the env list.
 * @param key   The environment variable key (e.g., "PWD").
 * @param value The new value to assign to the key.
 */
void	update_env_var(t_mini *mini, char *key, char *value)
{
	t_env	*env;

	env = find_env(mini->env, key);
	if (env)
	{
		free(env->val);
		env->val = ft_strdup(value);
	}
	else
		env_add_back(&mini->env, env_new(key, value, true));
}

/**
 * @fn bool print_escaped_val(char *val)
 * @brief Prints a variable value escaping special shell characters.
 * @details Used by `export` to output values in a way that they can be 
 * reused as valid input. Inside double quotes, the following must be escaped:
 * - Backslash (`\`) -> `\\`
 * - Double quote (`"`) -> `\"`
 * - Dollar sign (`$`) -> `\$` (Prevent variable expansion)
 * - Backtick (`` ` ``) -> `\` ` (Prevent command substitution)
 * @param val The string value to print.
 * @return    true on success, false on write error.
 */
bool	print_escaped_val(char *val)
{
	int	i;

	i = -1;
	while (val[++i])
	{
		if (val[i] == '\"' || val[i] == '\\' || val[i] == '$' || val[i] == '`')
		{
			if (write(STDOUT_FILENO, "\\", 1) < 0)
				return (false);
		}
		if (write(STDOUT_FILENO, &val[i], 1) < 0)
			return (false);
	}
	return (true);
}

/**
 * @fn char *get_old_pwd(t_mini *mini)
 * @brief Retrieves the current PWD to be saved as OLDPWD.
 * @details 
 * 1. Checks the "PWD" environment variable. If it exists and is valid, 
 * returns a copy of it.
 * 2. If "PWD" is unset, it attempts to use `getcwd` to get the physical path.
 * 3. Fallback: Returns an empty string if both fail (prevents returning NULL).
 * @param mini Pointer to the main shell structure.
 * @return     A newly allocated string containing the PWD (or empty string).
 */
char	*get_old_pwd(t_mini *mini)
{
	t_env	*env;
	char	*res;

	env = find_env(mini->env, "PWD");
	if (env && env->val)
		res = ft_strdup(env->val);
	else
		res = getcwd(NULL, 0);
	if (!res)
		res = ft_strdup("");
	return (res);
}

/**
 * @fn bool is_builtin(const char *cmd)
 * @brief Checks if a command is a shell builtin.
 * @details Compares the command string against the list of implemented 
 * builtins: cd, echo, env, exit, export, pwd, unset.
 * @param cmd The command name to check.
 * @return    true if the command is a builtin, false otherwise.
 */
bool	is_builtin(const char *cmd)
{
	if (!cmd)
		return (false);
	return (!ft_strcmp(cmd, "cd")
		|| !ft_strcmp(cmd, "echo")
		|| !ft_strcmp(cmd, "env")
		|| !ft_strcmp(cmd, "exit")
		|| !ft_strcmp(cmd, "export")
		|| !ft_strcmp(cmd, "pwd")
		|| !ft_strcmp(cmd, "unset"));
}
