/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 15:23:28 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/06 16:15:00 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/**
 * @fn t_env *find_env(t_env *env, char *key)
 * @brief Searches for an environment variable by key.
 * @details Traverses the linked list comparing keys.
 * @param env Pointer to the head of the environment list.
 * @param key The key string to search for (e.g., "PATH").
 * @return    Pointer to the found node, or NULL if not found.
 */
t_env	*find_env(t_env *env, char *key)
{
	while (env)
	{
		if (!ft_strcmp(env->key, key))
			return (env);
		env = env->next;
	}
	return (NULL);
}

/**
 * @fn int get_env_size(t_env *env)
 * @brief Calculates the number of elements in the environment list.
 * @details Traverses the linked list and counts all nodes that are marked 
 * as exported. Local shell variables are excluded from the count.
 * @param env Pointer to the head of the environment list.
 * @return    The number of exported environment variables.
 */
int	get_env_size(t_env *env)
{
	int	size;

	size = 0;
	while (env)
	{
		if (env->exported)
			size++;
		env = env->next;
	}
	return (size);
}

/**
 * @fn t_env *env_new(char *key, char *val, bool exported)
 * @brief Creates a new environment node.
 * @details Allocates memory for the node and duplicates the key and value. 
 * Initializes the 'next' pointer to NULL.
 * @param key      The key string to duplicate.
 * @param val      The value string to duplicate (can be NULL).
 * @param exported Boolean flag indicating if the variable is exported.
 * @return         Pointer to the new node, or NULL on allocation failure.
 */
t_env	*env_new(char *key, char *val, bool exported)
{
	t_env	*new_node;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = ft_strdup(key);
	if (!new_node->key)
		return (free(new_node), NULL);
	new_node->val = NULL;
	if (val)
	{
		new_node->val = ft_strdup(val);
		if (!new_node->val)
			return (free(new_node->key), free(new_node), NULL);
	}
	new_node->exported = exported;
	new_node->next = NULL;
	return (new_node);
}

/**
 * @fn void env_add_back(t_env **env, t_env *new)
 * @brief Appends a new node to the end of the environment list.
 * @details Traverses the environment linked list to the last node and 
 * links the new structure. If the list is empty, it initializes the head.
 * @param env Double pointer to the head of the list (to handle empty list 
 * case).
 * @param new Pointer to the new node to add.
 */
void	env_add_back(t_env **env, t_env *new)
{
	t_env	*node;

	if (!env || !new)
		return ;
	node = *env;
	if (!node)
	{
		*env = new;
		return ;
	}
	while (node->next)
		node = node->next;
	node->next = new;
}

/**
 * @fn void env_list_clear(t_env **env)
 * @brief Clears and frees the entire environment list.
 * @details Iterates through the environment list, safely freeing both 
 * the duplicated key/value strings and the node itself to prevent leaks.
 * @param env Double pointer to the head of the environment list.
 */
void	env_list_clear(t_env **env)
{
	t_env	*node;

	if (!env || !*env)
		return ;
	while (*env)
	{
		node = (*env)->next;
		free((*env)->key);
		free((*env)->val);
		free(*env);
		*env = node;
	}
}
