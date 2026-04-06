/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 20:10:16 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 04:54:37 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/**
 * @fn static char *join_key_value(t_env *env)
 * @brief Helper to join key and value into "KEY=VALUE" string.
 * @details Allocates memory for "KEY=VALUE" (or just "KEY" if value is NULL). 
 * Used by env_list_to_array to format each string.
 * @param env The environment node.
 * @return    Allocated string, or NULL on failure.
 */
static char	*join_key_value(t_env *env)
{
	char	*str;
	int		len;

	len = ft_strlen(env->key) + 1;
	if (env->val)
		len += ft_strlen(env->val) + 1;
	str = malloc(sizeof(char) * len);
	if (!str)
		return (NULL);
	ft_strlcpy(str, env->key, len);
	if (env->val)
	{
		ft_strlcat(str, "=", len);
		ft_strlcat(str, env->val, len);
	}
	return (str);
}

/**
 * @fn char **env_list_to_array(t_env *env)
 * @brief Converts the environment list to a string array.
 * @details Creates a NULL-terminated array of strings (char **) from the 
 * environment list. Only includes variables with 'exported' set to true. 
 * Reuses join_key_value for formatting.
 * @param env Pointer to the head of the environment list.
 * @return    Allocated array of strings, or NULL on failure.
 */
char	**env_list_to_array(t_env *env)
{
	char	**array;
	int		i;

	array = ft_calloc((get_env_size(env) + 1), sizeof(char *));
	if (!array)
		return (NULL);
	i = -1;
	while (env)
	{
		if (env->exported)
		{
			array[++i] = join_key_value(env);
			if (!array[i])
			{
				free_matrix(array);
				return (NULL);
			}
		}
		env = env->next;
	}
	return (array);
}

/**
 * @fn static int env_strcmp(const char *s1, const char *s2)
 * @brief Custom comparison for environment sorting.
 * @details Behaves like strcmp but treats '=' as a terminator (value 0). 
 * This ensures "TEST" (shorter key) comes before "TEST2" (longer key), 
 * because '=' (ASCII 61) is normally > '2' (ASCII 50).
 * @param s1 First string.
 * @param s2 Second string.
 * @return   Difference between characters.
 */
static int	env_strcmp(const char *s1, const char *s2)
{
	unsigned char	c1;
	unsigned char	c2;
	int				i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	c1 = (unsigned char)s1[i];
	c2 = (unsigned char)s2[i];
	if (c1 == '=')
		c1 = 0;
	if (c2 == '=')
		c2 = 0;
	return (c1 - c2);
}

/**
 * @fn void sort_env_array(char **envp, int size)
 * @brief Sorts a string array (char **) alphabetically.
 * @details Uses a simple bubble sort algorithm to rearrange the strings in the 
 * array. It swaps pointers rather than copying strings for efficiency.
 * @param envp The NULL-terminated array of strings to sort.
 * @param size The number of elements in the array.
 */
void	sort_env_array(char **envp, int size)
{
	char	*tmp;
	int		i;
	int		j;

	i = -1;
	while (++i < size - 1)
	{
		j = -1;
		while (++j < size - i - 1)
		{
			if (env_strcmp(envp[j], envp[j + 1]) > 0)
			{
				tmp = envp[j];
				envp[j] = envp[j + 1];
				envp[j + 1] = tmp;
			}
		}
	}
}
