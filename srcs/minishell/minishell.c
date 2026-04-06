/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 11:23:54 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:48:06 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "exec.h"
#include "expand.h"
#include "signals.h"

volatile sig_atomic_t	g_signal = 0;

/**
 * @fn static char *get_input_line(t_mini *mini)
 * @brief Handles prompt display and reading input.
 * @details Supports both interactive (readline) and non-interactive 
 * (get_next_line) modes. Handles GNL newline removal.
 * @param mini Pointer to the main shell structure.
 * @return     The line read, or NULL on EOF.
 */
static char	*get_input_line(t_mini *mini)
{
	char	*line;
	size_t	len;

	get_prompt_info(&mini->prompt, mini->env);
	if (isatty(STDIN_FILENO))
		return (readline(mini->prompt.login));
	line = get_next_line(STDIN_FILENO);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}

/**
 * @fn static char *get_last_arg(t_ast *ast)
 * @brief Retrieves the last argument of the last executed command.
 * @details Traverses the AST down the right-hand side (since execution flows 
 * left-to-right, the 'last' command is on the right).
 * - Logic/Pipe nodes: Recurse to rnode.
 * - Subshell: Recurse to the inner AST (lnode).
 * - Command: Return the last string in the argv array.
 * @param ast The root of the executed AST.
 * @return    The string value of the last argument, or NULL if none.
 */
static char	*get_last_arg(t_ast *ast)
{
	int		i;

	if (!ast)
		return (NULL);
	if (ast->type == AST_PIPE || ast->type == AST_AND || ast->type == AST_OR)
		return (get_last_arg(ast->rnode));
	if (ast->type == AST_SUBSHELL)
		return (get_last_arg(ast->lnode));
	if (ast->type == AST_CMD && ast->cmd && ast->cmd->argv)
	{
		i = 0;
		while (ast->cmd->argv[i])
			i++;
		if (i > 0)
			return (ast->cmd->argv[i - 1]);
	}
	return (NULL);
}

/**
 * @fn static void update_underscore(t_mini *mini)
 * @brief Updates the special parameter '_' env variable.
 * @details Sets '_' to the last argument of the previous command. 
 * This is done immediately after execution but before AST cleanup. 
 * If the variable doesn't exist, it creates it.
 * @param mini Pointer to the main shell structure.
 */
static void	update_underscore(t_mini *mini)
{
	char	*last_arg;
	t_env	*node;
	char	*new_val;

	last_arg = get_last_arg(mini->ast);
	if (!last_arg)
		return ;
	node = find_env(mini->env, "_");
	if (node)
	{
		new_val = ft_strdup(last_arg);
		if (new_val)
		{
			free(node->val);
			node->val = new_val;
		}
	}
	else
		env_add_back(&mini->env, env_new("_", last_arg, true));
}

/**
 * @fn static void process_input(t_mini *mini)
 * @brief Processes the input line: Tokenize -> Expand -> Parse -> Exec.
 * @details Orchestrates the shell's processing pipeline:
 * 1. Adds line to history (if interactive).
 * 2. Tokenizes the input.
 * 3. Expands variables.
 * 4. Parses tokens into an AST.
 * 5. Checks/handles heredocs.
 * 6. Executes the AST.
 * 7. Updates the `_` variable with the last argument.
 * 8. Cleans up heredoc temp files.
 * @param mini Pointer to the main shell structure.
 */
static void	process_input(t_mini *mini)
{
	if (isatty(STDIN_FILENO) && *mini->prompt.line != '\0')
		add_history(mini->prompt.line);
	if (!tokenizer(mini))
		return ;
	expand(mini);
	parser(mini);
	if (!mini->ast)
		return ;
	if (process_heredoc(mini, mini->ast))
	{
		mini->exit_code = exec(mini, mini->ast);
		update_underscore(mini);
	}
	else
		mini->exit_code = EXIT_SIGINT;
	cleanup_heredoc(mini->ast);
}

/**
 * @fn int main(int argc, char **argv, char **envp)
 * @brief Main shell loop.
 * @details Initializes the shell, enters an infinite loop to read and execute 
 * commands, and handles final cleanup upon exit.
 * @param argc Argument count (unused).
 * @param argv Argument values (unused).
 * @param envp Environment variables.
 * @return     The final exit status code.
 */
int	main(int argc, char **argv, char **envp)
{
	t_mini	mini;

	(void) argc;
	(void) argv;
	init_minishell(&mini, envp);
	while (true)
	{
		mini.prompt.line = get_input_line(&mini);
		if (g_signal != 0)
		{
			mini.exit_code = 128 + g_signal;
			g_signal = 0;
		}
		if (!mini.prompt.line)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		mini.line_count++;
		process_input(&mini);
		cleanup_iteration(&mini);
	}
	free_mini(&mini);
	return (mini.exit_code);
}
