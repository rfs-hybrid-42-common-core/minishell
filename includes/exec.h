/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:02:11 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 15:21:23 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "minishell.h"
# include "parser.h"
# include "signals.h"
# include "utils.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <string.h>
# include <sys/stat.h>
# include <unistd.h>

/* ========================================================================== */
/* TYPEDEFS                                                                   */
/* ========================================================================== */

/**
 * @typedef t_io
 * @brief Forward declaration of the IO backup structure.
 * @details Stores the original standard input and output file descriptors 
 * before any command redirections are applied. This allows the execution 
 * engine to safely run builtins in the main process and restore the 
 * original terminal I/O state afterwards.
 */
typedef struct s_io		t_io;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ------------------------- Execution Dispatchers -------------------------- */
int		exec(t_mini *mini, t_ast *ast);
int		exec_command(t_mini *mini, t_cmd *cmd);
int		exec_builtin(t_mini *mini, t_cmd *cmd);
int		exec_external(t_mini *mini, t_cmd *cmd, t_io *io);
int		exec_pipeline(t_mini *mini, t_ast *lnode, t_ast *rnode);
int		exec_logic(t_mini *mini, t_ast *ast);
int		exec_subshell(t_mini *mini, t_ast *ast);
int		exec_heredoc(t_mini *mini, int fd, char *lim, bool expand);

/* --------------------------- Local Assignments ---------------------------- */
bool	is_local_assignment(char **argv);
int		exec_local_assignment(t_mini *mini, t_cmd *cmd);

/* ------------------------------ Redirections ------------------------------ */
int		setup_redirections(t_redir *redirs);

/* -------------------------------- Heredoc --------------------------------- */
char	*get_raw_input(void);
void	print_heredoc(t_mini *mini, int fd, char *line, bool expand);
void	print_heredoc_warning(int line_count, char *lim);

/* --------------------------------- Utils ---------------------------------- */
int		exec_exit_status(int status);
bool	is_valid_key(char *key);
bool	is_directory(char *path);

#endif
