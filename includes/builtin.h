/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:00:41 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/05 19:56:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_H
# define BUILTIN_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "exec.h"
# include "minishell.h"
# include "utils.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <errno.h>
# include <limits.h>
# include <stdlib.h>
# include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* --------------------------- Builtins Handlers ---------------------------- */
int		builtin_cd(t_mini *mini, char **argv);
int		builtin_echo(t_mini *mini, char **argv);
int		builtin_env(t_mini *mini, char **argv);
int		builtin_exit(t_mini *mini, char **argv);
int		builtin_export(t_mini *mini, char **argv);
int		builtin_pwd(t_mini *mini, char **argv);
int		builtin_unset(t_mini *mini, char **argv);

/* --------------------------- Dispatcher & Utils --------------------------- */
void	update_env_var(t_mini *mini, char *key, char *value);
bool	print_escaped_val(char *val);
char	*get_old_pwd(t_mini *mini);
bool	is_builtin(const char *cmd);

#endif
