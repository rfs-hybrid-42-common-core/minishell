/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isspace.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 13:58:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/21 15:38:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @fn int ft_isspace(int c)
 * @brief Checks for white-space characters.
 * @details Includes: space, form-feed ('\f'), newline ('\n'), 
 * carriage return ('\r'), horizontal tab ('\t'), and vertical tab ('\v').
 * @param c The character to test.
 * @return  Non-zero if the character is whitespace, 0 otherwise.
 */
int	ft_isspace(int c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}
