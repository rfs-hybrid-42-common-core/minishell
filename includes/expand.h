/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 18:36:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 15:33:35 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPAND_H
# define EXPAND_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "libft.h"
# include "minishell.h"
# include "token.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <dirent.h>
# include <stdbool.h>
# include <stdlib.h>
# include <sys/types.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def WILDCARD
 * @brief Sentinel character used internally during wildcard expansion.
 * @details An unprintable ASCII character ('\002') used to differentiate 
 * a literal '*' enclosed in quotes from an unquoted '*' that should trigger 
 * globbing.
 */
# define WILDCARD	'\002'

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ---------------------------- Expansion Logic ----------------------------- */
void	expand_tilde(t_mini *mini, t_token *token);
char	*expand_heredoc(t_mini *mini, char *str);
bool	expand_variable(t_mini *mini, t_token **curr, t_token *prev,
			t_token *next);
void	expand_split(t_token **curr, char *str);
t_token	*expand_wildcard(t_token **tokens, t_token *curr, t_token **prev,
			bool *error);
void	expand(t_mini *mini);

/* ------------------------------ Expand Utils ------------------------------ */
bool	toggle_quotes(char c, bool *sq, bool *dq, int *index);
bool	remove_empty_token(t_mini *mini, t_token **curr, t_token *prev,
			t_token *next);
bool	remove_backlash(char c, bool sq, bool dq);
char	*append_str(char *s1, char *s2);
char	*get_var_value(t_mini *mini, char *str, int *index);

/* ----------------------------- Wildcard Utils ----------------------------- */
bool	has_wildcard(t_token *token);
char	*get_wildcard_str(char *str);
int		wildcard_token_count(t_token *tokens);

#endif
