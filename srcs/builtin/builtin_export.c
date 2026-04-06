/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:01:30 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:37:58 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

/**
 * @fn static bool print_line(char *str)
 * @brief Helper to print a single exported variable.
 * @details 
 * - Format: `declare -x KEY="VAL"`
 * - Uses `print_escaped_val` to handle special characters inside the value.
 * - Uses strict write() calls to detect output errors (e.g., broken pipe).
 * @param str The environment string "KEY=VAL" or "KEY".
 * @return    true on success, false on write error.
 */
static bool	print_line(char *str)
{
	char	*eq;

	eq = ft_strchr(str, '=');
	if (write(STDOUT_FILENO, "declare -x ", 11) < 0)
		return (print_write_error("export"), false);
	if (eq)
	{
		if (write(STDOUT_FILENO, str, eq - str) < 0
			|| write(STDOUT_FILENO, "=\"", 2) < 0)
			return (print_write_error("export"), false);
		if (!print_escaped_val(eq + 1))
			return (print_write_error("export"), false);
		if (write(STDOUT_FILENO, "\"\n", 2) < 0)
			return (print_write_error("export"), false);
	}
	else
	{
		if (write(STDOUT_FILENO, str, ft_strlen(str)) < 0
			|| write(STDOUT_FILENO, "\n", 1) < 0)
			return (print_write_error("export"), false);
	}
	return (true);
}

/**
 * @fn static int print_exported(t_mini *mini)
 * @brief Driver function to print all exported variables.
 * @details 
 * 1. Converts the environment list to a string array.
 * 2. Sorts the array alphabetically (Bash requirement).
 * 3. Iterates and prints each variable using `print_line`, skipping "_".
 * 4. Frees the temporary array.
 * @param mini Pointer to the main minishell structure.
 * @return     EXIT_SUCCESS, or EXIT_FAILURE on malloc/write error.
 */
static int	print_exported(t_mini *mini)
{
	char	**envp;
	int		i;

	envp = env_list_to_array(mini->env);
	if (!envp)
		return (EXIT_FAILURE);
	sort_env_array(envp, get_env_size(mini->env));
	i = -1;
	while (envp[++i])
	{
		if (ft_strncmp(envp[i], "_=", 2) && ft_strcmp(envp[i], "_"))
		{
			if (!print_line(envp[i]))
			{
				free_matrix(envp);
				return (EXIT_FAILURE);
			}
		}
	}
	free_matrix(envp);
	return (EXIT_SUCCESS);
}

/**
 * @fn static void update_env_export(t_mini *mini, char *key, char *val, 
 * bool append)
 * @brief Adds or updates an environment variable with export status.
 * @details 
 * - If key exists: Updates value (overwrite or append).
 * - If key is new: Creates new node.
 * - Always marks the node as `exported = true`.
 * @param mini   Pointer to main shell structure.
 * @param key    The variable key.
 * @param val    The value to assign (can be NULL).
 * @param append Flag indicating if value should be appended (+=).
 */
static void	update_env_export(t_mini *mini, char *key, char *val, bool append)
{
	t_env	*node;
	char	*tmp;

	node = find_env(mini->env, key);
	if (!node)
	{
		env_add_back(&mini->env, env_new(key, val, true));
		return ;
	}
	if (val && append && node->val)
	{
		tmp = ft_strjoin(node->val, val);
		free(node->val);
		node->val = tmp;
	}
	else if (val)
	{
		if (node->val)
			free(node->val);
		node->val = ft_strdup(val);
	}
	node->exported = true;
	return ;
}

/**
 * @fn static bool process_export(t_mini *mini, char *arg)
 * @brief Processes a single export argument.
 * @details 
 * 1. Parses the argument to separate Key, Value, and Append mode (+=).
 * 2. Validates the Key syntax.
 * 3. Updates the environment.
 * @param mini Pointer to the main minishell structure.
 * @param arg  The argument string (e.g., "VAR=value", "VAR+=val", or "VAR").
 * @return     true on success, false on invalid identifier.
 */
static bool	process_export(t_mini *mini, char *arg)
{
	char	*key;
	char	*val;
	char	*eq;
	bool	append;

	eq = ft_strchr(arg, '=');
	append = (eq && eq > arg && eq[-1] == '+');
	val = NULL;
	if (eq)
	{
		key = ft_substr(arg, 0, (eq - arg) - append);
		val = ft_strdup(eq + 1);
	}
	else
		key = ft_strdup(arg);
	if (!is_valid_key(key))
	{
		print_export_error(arg, "not a valid identifier");
		return (free(key), free(val), false);
	}
	update_env_export(mini, key, val, append);
	return (free(key), free(val), true);
}

/**
 * @fn int builtin_export(t_mini *mini, char **argv)
 * @brief Handles the export builtin command.
 * @details 
 * - No args: Prints sorted list of exported variables.
 * - With args: Loops through arguments and processes assignments.
 * - Sets exit status to failure if ANY argument is invalid.
 * @param mini Pointer to the main minishell structure.
 * @param argv Command arguments.
 * @return     EXIT_SUCCESS if all args valid, EXIT_FAILURE otherwise.
 */
int	builtin_export(t_mini *mini, char **argv)
{
	int	exit_code;
	int	i;

	exit_code = EXIT_SUCCESS;
	if (!argv[1])
		return (print_exported(mini));
	i = 1;
	while (argv[i])
	{
		if (!process_export(mini, argv[i]))
			exit_code = EXIT_FAILURE;
		i++;
	}
	return (exit_code);
}
