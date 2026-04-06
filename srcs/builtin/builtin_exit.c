/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:02:08 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 16:25:29 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "parser.h"
#include "token.h"

/**
 * @fn static void exit_with_cleanup(t_mini *mini, int exit_code)
 * @brief Helper to free all resources and exit the shell.
 * @details Calls the iteration cleanup (AST, Tokens) and the global cleanup 
 * (Environment, History) before terminating the process.
 * @param mini      Pointer to the main shell structure.
 * @param exit_code The status code to exit with.
 */
static void	exit_with_cleanup(t_mini *mini, int exit_code)
{
	cleanup_iteration(mini);
	free_mini(mini);
	exit(exit_code);
}

/**
 * @fn static bool is_overflow(unsigned long long result, int sign, int digit)
 * @brief Checks if adding a digit would cause a long long overflow.
 * @details Compares the current accumulated result against the LONG_MAX 
 * and LONG_MIN limits before the next digit is added, ensuring the conversion 
 * remains within valid 64-bit signed integer bounds.
 * @param result The current accumulated number.
 * @param sign   The sign of the number (1 or -1).
 * @param digit  The next digit to add.
 * @return       true if overflow occurs, false otherwise.
 */
static bool	is_overflow(unsigned long long result, int sign, int digit)
{
	if (sign == 1)
	{
		if (result > (unsigned long long)LLONG_MAX / 10
			|| (result == (unsigned long long)LLONG_MAX / 10
				&& digit > LLONG_MAX % 10))
			return (true);
	}
	else
	{
		if (result > (unsigned long long)LLONG_MAX / 10
			|| (result == (unsigned long long)LLONG_MAX / 10
				&& digit > (LLONG_MAX % 10) + 1))
			return (true);
	}
	return (false);
}

/**
 * @fn static long long ft_atoll_check(const char *str, bool *error)
 * @brief Converts a string to a long long integer with strict error checking.
 * @details Handles sign, checks for non-numeric characters, overflow, 
 * and trailing garbage. Sets the error flag if any issue is found.
 * @param str   The string to convert.
 * @param error Pointer to a boolean flag to set on error.
 * @return      The converted value (undefined if error is set).
 */
static long long	ft_atoll_check(const char *str, bool *error)
{
	unsigned long long	result;
	int					sign;

	result = 0;
	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
		if (*str++ == '-')
			sign *= -1;
	if (!ft_isdigit(*str))
		*error = true;
	while (ft_isdigit(*str))
	{
		if (is_overflow(result, sign, *str - '0'))
			*error = true;
		result = result * 10 + (*str++ - '0');
	}
	while (ft_isspace(*str))
		str++;
	if (*str)
		*error = true;
	return ((long long)(result * sign));
}

/**
 * @fn static bool is_main_process(t_mini *mini)
 * @brief Checks if the current process is the main shell instance.
 * @details Reads the current PID from /proc/self/stat and compares it 
 * with the PID stored in mini->pid_str (initialized at startup).
 * This prevents "exit" from being printed in subshells (e.g., (exit 1)).
 * @param mini Pointer to the main shell structure.
 * @return     true if current PID matches main shell PID, false otherwise.
 */
static bool	is_main_process(t_mini *mini)
{
	char	buffer[PID_SIZE];
	int		fd;
	int		i;
	int		len;

	fd = open("/proc/self/stat", O_RDONLY);
	if (fd == -1)
		return (false);
	len = read(fd, buffer, PID_SIZE - 1);
	close(fd);
	if (len <= 0)
		return (false);
	buffer[len] = '\0';
	i = 0;
	while (buffer[i] && ft_isdigit(buffer[i]))
		i++;
	buffer[i] = '\0';
	if (!ft_strncmp(buffer, mini->pid_str, ft_strlen(mini->pid_str) + 1))
		return (true);
	return (false);
}

/**
 * @fn int builtin_exit(t_mini *mini, char **argv)
 * @brief Handles the 'exit' builtin command.
 * @details 
 * 1. Checks if running in main process to decide whether to print "exit".
 * 2. No args: Exits with the status of the last executed command.
 * 3. Non-numeric arg: Prints error, cleans up, and exits with 2.
 * 4. Too many args: Prints error and returns failure (shell continues).
 * 5. Numeric arg: Exits with (arg % 256).
 * @param mini Pointer to the main shell structure.
 * @param argv Command arguments.
 * @return     EXIT_FAILURE/Status if "too many arguments", otherwise
 * terminates.
 */
int	builtin_exit(t_mini *mini, char **argv)
{
	long long	exit_code;
	bool		error;

	if (is_main_process(mini) && isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!argv[1])
		exit_with_cleanup(mini, mini->exit_code);
	error = false;
	exit_code = ft_atoll_check(argv[1], &error);
	if (error)
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(argv[1], STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		exit_with_cleanup(mini, EXIT_SYNTAX_ERROR);
	}
	if (argv[2])
	{
		print_command_error("exit", "too many arguments");
		if (mini->exit_code != 0)
			return (mini->exit_code);
		return (EXIT_FAILURE);
	}
	exit_with_cleanup(mini, exit_code % 256);
	return (EXIT_SUCCESS);
}
