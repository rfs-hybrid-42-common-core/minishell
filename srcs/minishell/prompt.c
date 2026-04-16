/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 12:24:55 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/16 16:53:29 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @fn static char *get_username(t_env *env)
 * @brief Retrieves the current user from the environment variables.
 * @details Searches for the "USER" key in the environment list.
 * @param env Pointer to the environment list.
 * @return    Allocated string containing the username, or "minishell" default.
 */
static char	*get_username(t_env *env)
{
	while (env)
	{
		if (!ft_strcmp(env->key, "USER"))
		{
			if (env->val)
				return (ft_strdup(env->val));
			return (ft_strdup("minishell"));
		}
		env = env->next;
	}
	return (ft_strdup("minishell"));
}

/**
 * @fn static char *get_hostname(void)
 * @brief Retrieves the system hostname.
 * @details Reads from /etc/hostname directly into a small buffer. 
 * Replaces get_next_line with a single read() call for performance. 
 * Truncates at the first '.' to get the short hostname.
 * @return Allocated string containing the hostname, or "localhost" on error.
 */
static char	*get_hostname(void)
{
	int		fd;
	ssize_t	bytes;
	char	host[BUFFER_SIZE];
	char	*dot;

	fd = open("/etc/hostname", O_RDONLY);
	if (fd < 0)
		return (ft_strdup("localhost"));
	bytes = read(fd, host, BUFFER_SIZE - 1);
	close (fd);
	if (bytes <= 0)
		return (ft_strdup("localhost"));
	host[bytes] = '\0';
	if (host[bytes - 1] == '\n')
		host[bytes - 1] = '\0';
	dot = ft_strchr(host, '.');
	if (dot)
		*dot = '\0';
	return (ft_strdup(host));
}

/**
 * @fn static char *get_raw_cwd(t_env *env)
 * @brief Retrieves the raw current working directory.
 * @details 
 * 1. Attempts to get the physical path via getcwd().
 * 2. If getcwd() fails (e.g., directory deleted), searches for "PWD" in the 
 * environment variables.
 * 3. If "PWD" is not found, defaults to "." as a last resort.
 * @param env Pointer to the environment list (to find PWD).
 * @return    Allocated string containing the raw path.
 */
static char	*get_raw_cwd(t_env *env)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
		return (cwd);
	while (env)
	{
		if (!ft_strcmp(env->key, "PWD"))
			return (ft_strdup(env->val));
		env = env->next;
	}
	return (ft_strdup("."));
}

/**
 * @fn static char *get_dir(t_env *env)
 * @brief Formats the working directory string for the prompt.
 * @details 
 * 1. Calls get_raw_cwd() to retrieve the base path (handling deleted dirs).
 * 2. Checks if the path starts with the user's HOME directory.
 * 3. If yes, replaces the HOME prefix with "~" (e.g., /home/user -> ~).
 * @param env Pointer to the environment list (to find HOME).
 * @return    Allocated string containing the formatted path.
 */
static char	*get_dir(t_env *env)
{
	char	*cwd;
	char	*home;
	t_env	*tmp;
	size_t	len;

	cwd = get_raw_cwd(env);
	tmp = env;
	while (tmp)
	{
		if (!ft_strcmp(tmp->key, "HOME"))
			break ;
		tmp = tmp->next;
	}
	if (tmp && tmp->val && *tmp->val && cwd)
	{
		len = ft_strlen(tmp->val);
		if (!ft_strncmp(cwd, tmp->val, len)
			&& (cwd[len] == '\0' || cwd[len] == '/'))
		{
			home = ft_strjoin("~", cwd + ft_strlen(tmp->val));
			if (home)
				return (free(cwd), home);
		}
	}
	return (cwd);
}

/**
 * @fn void get_prompt_info(t_prompt *prompt, t_env *env)
 * @brief Builds the shell prompt string (user@host:$ ).
 * @details Concatenates username and hostname into a formatted string. 
 * Stores the result in prompt->login.
 * @param prompt Pointer to the prompt structure to populate.
 * @param env    Pointer to the environment list (to find USER).
 */
void	get_prompt_info(t_prompt *prompt, t_env *env)
{
	char	*info[3];
	size_t	len;

	info[0] = get_username(env);
	info[1] = get_hostname();
	info[2] = get_dir(env);
	if (!info[0] || !info[1] || !info[2])
		return (free(info[0]), free(info[1]), free(info[2]));
	len = ft_strlen(info[0]) + ft_strlen(info[1]) + ft_strlen(info[2]) + 5;
	len += ft_strlen(LOG_GREEN) + ft_strlen(LOG_BLUE) + ft_strlen(LOG_RST) * 2;
	free(prompt->login);
	prompt->login = ft_calloc(len, sizeof(char));
	if (!prompt->login)
		return (free(info[0]), free(info[1]), free(info[2]));
	ft_strlcat(prompt->login, LOG_GREEN, len);
	ft_strlcat(prompt->login, info[0], len);
	ft_strlcat(prompt->login, "@", len);
	ft_strlcat(prompt->login, info[1], len);
	ft_strlcat(prompt->login, LOG_RST, len);
	ft_strlcat(prompt->login, ":", len);
	ft_strlcat(prompt->login, LOG_BLUE, len);
	ft_strlcat(prompt->login, info[2], len);
	ft_strlcat(prompt->login, LOG_RST, len);
	ft_strlcat(prompt->login, "$ ", len);
	return (free(info[0]), free(info[1]), free(info[2]));
}
