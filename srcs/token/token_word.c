/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:19:22 by rjorge-p          #+#    #+#             */
/*   Updated: 2026/04/06 04:53:43 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token.h"

/**
 * @fn static char *single_quotes(char *word, char *input, int *index)
 * @brief Handles a single-quoted string segment.
 * @details Appends characters literally until the closing quote is found. 
 * Does not interpret any special characters inside the quotes.
 * @param word  The current word being built.
 * @param input The full input string.
 * @param index Pointer to the current index in the input string.
 * @return      The updated word, or NULL if quotes are unclosed.
 */
static char	*single_quotes(char *word, char *input, int *index)
{
	word = append_char(word, input[*index]);
	(*index)++;
	while (input[*index] != '\0' && input[*index] != '\'')
	{
		word = append_char(word, input[*index]);
		if (!word)
			return (NULL);
		(*index)++;
	}
	if (input[*index] != '\'')
	{
		free(word);
		return (NULL);
	}
	word = append_char(word, input[*index]);
	(*index)++;
	return (word);
}

/**
 * @fn static char *double_quotes(char *word, char *input, int *index)
 * @brief Handles a double-quoted string segment.
 * @details Appends characters until the closing quote. 
 * Interprets escape characters only for '$', '"', '\', and newline, 
 * preserving the backslash literal for other characters.
 * @param word  The current word being built.
 * @param input The full input string.
 * @param index Pointer to the current index in the input string.
 * @return      The updated word, or NULL if quotes are unclosed.
 */
static char	*double_quotes(char *word, char *input, int *index)
{
	word = append_char(word, input[*index]);
	(*index)++;
	while (input[*index] != '\0' && input[*index] != '"')
	{
		if (input[*index] == '\\' && (input[*index + 1] == '"'
				|| input[*index + 1] == '\\' || input[*index + 1] == '$'))
		{
			word = append_char(word, input[*index]);
			(*index)++;
			word = append_char(word, input[*index]);
			(*index)++;
		}
		else
		{
			word = append_char(word, input[*index]);
			(*index)++;
		}
		if (!word)
			return (NULL);
	}
	if (input[*index] == '\0')
		return (free(word), NULL);
	word = append_char(word, input[*index]);
	(*index)++;
	return (word);
}

/**
 * @fn static char *escape_character(char *word, char *input, int *index)
 * @brief Handles a backslash escape outside of quotes.
 * @details Appends the backslash and the immediately following character 
 * literally to the word.
 * @param word  The current word being built.
 * @param input The full input string.
 * @param index Pointer to the current index in the input string.
 * @return      The updated word.
 */
static char	*escape_character(char *word, char *input, int *index)
{
	word = append_char(word, input[*index]);
	if (!word)
		return (NULL);
	(*index)++;
	if (input[*index] != '\0')
	{
		word = append_char(word, input[*index]);
		(*index)++;
	}
	return (word);
}

/**
 * @fn char *collect_word(char *input, int *index)
 * @brief Extracts a full word token from the input.
 * @details Iterates through the input, handling quotes and escapes, until 
 * a delimiter (space or operator) is reached.
 * @param input The full input string.
 * @param index Pointer to the current index in the input string.
 * @return      The newly allocated word string, or NULL on error.
 */
char	*collect_word(char *input, int *index)
{
	char	*word;

	word = ft_strdup("");
	if (!word)
		return (NULL);
	while (input[*index] && !ft_isspace(input[*index]) && !is_op(input[*index]))
	{
		if (input[*index] == '\\')
			word = escape_character(word, input, index);
		else if (input[*index] == '\'')
			word = single_quotes(word, input, index);
		else if (input[*index] == '"')
			word = double_quotes(word, input, index);
		else
		{
			word = append_char(word, input[*index]);
			(*index)++;
		}
		if (!word)
			return (NULL);
	}
	return (word);
}
