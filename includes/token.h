/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 15:05:18 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 15:51:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_H
# define TOKEN_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdbool.h>
# include <stdlib.h>

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_mini
 * @brief Forward declaration of the main shell control structure.
 * @details Holds the entire state of the running shell. It is passed down 
 * to the tokenizer so the lexer can access the raw input line from the 
 * prompt and directly build the token linked list within the main state.
 */
typedef struct s_mini	t_mini;

/**
 * @enum e_token_type
 * @brief Identifies the syntactic role of a token.
 * @details Used by the parser to classify the raw input chunks, 
 * differentiating between standard command words and shell 
 * operators (pipes, redirections, logic, parenthesis).
 * @var TOKEN_WORD           A standard word, command name, or argument.
 * @var TOKEN_PIPE           The pipe operator '|'.
 * @var TOKEN_AND            The logical AND operator '&&'.
 * @var TOKEN_OR             The logical OR operator '||'.
 * @var TOKEN_LPAREN         The left parenthesis '(' used to open a subshell.
 * @var TOKEN_RPAREN         The right parenthesis ')' used to close a subshell.
 * @var TOKEN_REDIR_IN       The input redirection operator '<'.
 * @var TOKEN_REDIR_OUT      The output redirection operator '>'.
 * @var TOKEN_APPEND         The output append redirection operator '>>'.
 * @var TOKEN_HEREDOC        The unquoted Here-Document operator '<<' (allows 
 * variable expansion).
 * @var TOKEN_HEREDOC_QUOTED The quoted Here-Document operator (prevents 
 * variable expansion).
 * @var TOKEN_END            Sentinel value marking the end of the token list.
 */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
	TOKEN_HEREDOC_QUOTED,
	TOKEN_END
}	t_token_type;

/**
 * @struct s_token
 * @brief Linked list node representing a parsed token.
 * @details Serves as the fundamental unit of parsed input generated 
 * by the lexer. This linear list is subsequently consumed by the 
 * parser to build the AST.
 * @var type The type of the token (operator or word).
 * @var val  The string content of the token (e.g., "ls", "|", "filename").
 * @var next Pointer to the next token.
 */
typedef struct s_token
{
	t_token_type	type;
	char			*val;
	struct s_token	*next;
}	t_token;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Main Tokenizer ----------------------------- */
bool	tokenizer(t_mini *mini);

/* --------------------------- List Manipulation ---------------------------- */
t_token	*token_new(t_token_type type, char *value);
void	token_add_back(t_token **tokens, t_token *new);
void	token_sort(t_token **tokens);

/* ----------------------------- Word Handling ------------------------------ */
char	*append_char(char *s, char c);
char	*collect_word(char *input, int *i);

/* --------------------------------- Utils ---------------------------------- */
int		is_op(char c);

#endif
