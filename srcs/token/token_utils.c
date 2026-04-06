/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 16:08:45 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 16:38:15 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token.h"

/**
 * @fn char *append_char(char *s, char c)
 * @brief Appends a single character to a dynamically allocated string.
 * @details Allocates a new string of size len+2, copies the old string, adds 
 * the character, and frees the old string.
 * @param s The original string (can be NULL).
 * @param c The character to append.
 * @return  The new string, or NULL on allocation failure.
 */
char	*append_char(char *s, char c)
{
	char	*new;
	size_t	len;

	len = 0;
	if (s != NULL)
		len = ft_strlen(s);
	new = malloc(sizeof(char) * (len + 2));
	if (!new)
	{
		free(s);
		return (NULL);
	}
	if (s != NULL)
		ft_memcpy(new, s, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(s);
	return (new);
}

/**
 * @fn int is_op(char c)
 * @brief Checks if a character is a shell operator delimiter.
 * @details Used during the word collection phase to determine where a standard 
 * token ends. Characters like pipes and logical ORs ('|'), redirections 
 * ('<', '>'), logical ANDs ('&'), and parenthesis ('(', ')') act as natural 
 * word delimiters in Bash.
 * @param c The character to check.
 * @return  1 if it is an operator, 0 otherwise.
 */
int	is_op(char c)
{
	return (c == '|' || c == '<' || c == '>'
		|| c == '&' || c == '(' || c == ')');
}

/**
 * @fn t_token *token_new(t_token_type type, char *value)
 * @brief Allocates and initializes a new token node.
 * @details Dynamically allocates memory for a new token, assigning its 
 * syntactic type and the exact string value parsed from the input. 
 * Initializes the next pointer to NULL.
 * @param type  The type of the token.
 * @param value The string value of the token (optional).
 * @return      Pointer to the new token, or NULL on failure.
 */
t_token	*token_new(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->val = value;
	token->next = NULL;
	return (token);
}

/**
 * @fn void token_add_back(t_token **tokens, t_token *new)
 * @brief Adds a token to the end of the linked list.
 * @details Traverses the existing token list to find the tail, then appends 
 * the newly created token. If the list is empty, it sets the new token as 
 * the head.
 * @param tokens Double pointer to the list head.
 * @param new    Pointer to the token to add.
 */
void	token_add_back(t_token **tokens, t_token *new)
{
	t_token	*node;

	if (!tokens || !new)
		return ;
	node = *tokens;
	if (!node)
	{
		*tokens = new;
		return ;
	}
	while (node->next)
		node = node->next;
	node->next = new;
}

/**
 * @fn void token_sort(t_token **tokens)
 * @brief Sorts the token list alphabetically by value.
 * @details Uses bubble sort to swap values between nodes.
 * @param tokens Double pointer to the list head.
 */
void	token_sort(t_token **tokens)
{
	t_token	*token;
	t_token	*curr;
	char	*tmp;

	if (!tokens || !*tokens)
		return ;
	token = *tokens;
	while (token)
	{
		curr = token->next;
		while (curr)
		{
			if (ft_strcmp(token->val, curr->val) > 0)
			{
				tmp = token->val;
				token->val = curr->val;
				curr->val = tmp;
			}
			curr = curr->next;
		}
		token = token->next;
	}
}
