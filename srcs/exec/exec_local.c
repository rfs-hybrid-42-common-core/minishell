/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_local.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 15:30:25 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:26:43 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @fn static bool check_assign_syntax(char *arg, char *eq)
 * @brief Validates the assignment syntax, including append mode (+=).
 * @details 
 * - Checks if the operator is `+=` or just `=`.
 * - Temporarily masks the operator with `\0` to validate the key using 
 * `is_valid_key`.
 * - Restores the operator after validation.
 * @param arg The full argument string.
 * @param eq  Pointer to the first '=' character in the string.
 * @return    true if valid key, false otherwise.
 */
static bool	check_assign_syntax(char *arg, char *eq)
{
	bool	is_valid;
	bool	append;

	append = (eq > arg && *(eq - 1) == '+');
	if (append)
	{
		*(eq - 1) = '\0';
		is_valid = is_valid_key(arg);
		*(eq - 1) = '+';
	}
	else
	{
		*eq = '\0';
		is_valid = is_valid_key(arg);
		*eq = '=';
	}
	return (is_valid);
}

/**
 * @fn bool is_local_assignment(char **argv)
 * @brief Checks if a command consists entirely of local variable assignments.
 * @details Iterates through the argument vector. For the command to be 
 * considered a local assignment (e.g., "A=1 B+=2"), EVERY argument must 
 * contain a valid key followed by `=` or `+=`.
 * @param argv The null-terminated array of command arguments.
 * @return     true if all arguments are valid assignments, false otherwise.
 */
bool	is_local_assignment(char **argv)
{
	int		i;
	char	*eq;

	i = 0;
	while (argv[i])
	{
		eq = ft_strchr(argv[i], '=');
		if (!eq)
			return (false);
		if (!check_assign_syntax(argv[i], eq))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @fn static void update_existing_var(t_env *node, char *val, bool append)
 * @brief Updates the value of an existing environment variable.
 * @details 
 * - If append mode (+=) is active: Concatenates new val to old val.
 * - Otherwise: Frees old val and replaces it with the new one.
 * @param node   Pointer to the environment node to update.
 * @param val    The new value string (to assign or append).
 * @param append Boolean flag for append mode.
 */
static void	update_existing_var(t_env *node, char *val, bool append)
{
	char	*tmp;

	if (append && node->val)
	{
		tmp = ft_strjoin(node->val, val);
		free(node->val);
		node->val = tmp;
	}
	else
	{
		free(node->val);
		node->val = ft_strdup(val);
	}
}

/**
 * @fn static void process_local_assignment(t_mini *mini, char *str)
 * @brief Helper to process a single assignment string.
 * @details Parses a string in "KEY=VALUE" or "KEY+=VALUE" format.
 * 1. Identifies the operator and separates Key and Value.
 * 2. Checks if the variable already exists in the environment.
 * 3. Calls `update_existing_var` if found, or adds a new node if not.
 * 4. Ensures local variables are marked as not exported (unless already 
 * exported, which is handled by preserving the existing node).
 * @param mini Pointer to the main minishell structure.
 * @param str  The assignment string to process (e.g., "VAR=hello").
 */
static void	process_local_assignment(t_mini *mini, char *str)
{
	t_env	*env;
	char	*key;
	char	*val;
	char	*eq;
	bool	append;

	eq = ft_strchr(str, '=');
	append = (eq > str && *(eq - 1) == '+');
	if (append)
		key = ft_substr(str, 0, (eq - str) - 1);
	else
		key = ft_substr(str, 0, eq - str);
	val = ft_strdup(eq + 1);
	env = find_env(mini->env, key);
	if (env)
		update_existing_var(env, val, append);
	else
		env_add_back(&mini->env, env_new(key, val, false));
	free(key);
	free(val);
}

/**
 * @fn int exec_local_assignment(t_mini *mini, t_cmd *cmd)
 * @brief Executes local variable assignments.
 * @details Iterates through the arguments and calls the processor for each. 
 * Handles multiple assignments in one command (e.g., `A=1 B=2`).
 * @param mini Pointer to the main minishell structure.
 * @param cmd  Pointer to the command structure.
 * @return     EXIT_SUCCESS.
 */
int	exec_local_assignment(t_mini *mini, t_cmd *cmd)
{
	int	i;

	i = -1;
	while (cmd->argv[++i])
		process_local_assignment(mini, cmd->argv[i]);
	return (EXIT_SUCCESS);
}
