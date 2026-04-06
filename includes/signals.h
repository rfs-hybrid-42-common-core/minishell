/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 17:01:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/05 20:48:04 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "minishell.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>

/* ========================================================================== */
/* GLOBALS                                                                    */
/* ========================================================================== */

/**
 * @var g_signal
 * @brief Global variable to communicate signal reception to the main loop.
 * @details Volatile sig_atomic_t ensures signal-safety.
 */
extern volatile sig_atomic_t	g_signal;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ------------------------- Signal Setup Functions ------------------------- */
void	setup_parent_signals(void);
void	setup_child_signals(void);
void	setup_signals(void);

#endif
