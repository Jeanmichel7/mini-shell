/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ydumaine <ydumaine@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 17:13:01 by ydumaine          #+#    #+#             */
/*   Updated: 2022/05/31 13:22:48 by ydumaine         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_omit_quote_apostrophe(char c, unsigned int omit, unsigned int *i, int keep_quotes)
{
		if (c == 34 && omit == 1)
		{
			if (keep_quotes == 0 && i != NULL)
				(*i)++;
			return (0);
		}
		if (c == 39 && omit == 2)
		{
			if (keep_quotes == 0 && i != NULL)
				(*i)++;
			return (0);
		}
		if (c == 34 && omit == 0)
		{
			if (keep_quotes == 0 && i != NULL)
				(*i)++;
			return (1);
		}
		if (c == 39 && omit == 0)
		{
			if (keep_quotes == 0 && i != NULL)
				(*i)++;
			return (2);
		}
		if (c == 39 && omit == 1)
			return (1); 
		if (c == 34 && omit == 2)
			return (2);
	return (omit);
}

int	ft_ycheck_pipe(char *temp)
{
	unsigned int	i;
	unsigned int	u;
	unsigned int 	omit;

	i = 0;
	u = 0;
	omit = 0; 
	while (temp[i])
	{
		omit = ft_omit_quote_apostrophe(temp[i], omit, &i, 1);
		if (temp[i] == '|' && omit == 0)
		{
			u++;
			i++;
			if (temp[i] == '|')
				return (-1);
			while (temp[i] == ' ')
				i++;
			if (temp[i] == '|')
				return (-1);
		}
		else
			i++;
	}
	return (u);
}

char	*ft_if_quotes_not_closes(t_data *data)
{
    char 	*ptr;
	char	 *temp; 	
	int 	i; 
	int		omit;

	i = -1;
	omit = 0;
	while (data->temp[++i])
		omit = 	ft_omit_quote_apostrophe(data->temp[i], omit, NULL, 1);
	if (omit != 0)
	{
		temp = data->temp; 
		data->temp = ft_strjoin_andadd_rt(data->temp, NULL);
		free(temp);
		while (omit != 0)
		{
			ptr = readline(">");
			i = -1;
			while (ptr[++i])
				omit = 	ft_omit_quote_apostrophe(ptr[i], omit, NULL, 1);
			temp = data->temp; 
			if (omit != 0)
				data->temp = ft_strjoin_andadd_rt(data->temp, ptr);
			else 
				data->temp = ft_strjoin(data->temp, ptr);
			if (data->temp == NULL)
				return (NULL);
			free(temp);
			free(ptr);
			printf("\n valeur de data->temp  : %s", data->temp);
		}
		i = 0;
	}
	return (data->temp);
}

int	ft_freetab(char **tab)
{
	int	i;
	i = 0;

	while (tab[i] != NULL)
		free(tab[i++]);
	free(tab);
	return (1);
}


char **ft_envcpy(char **env)
{
	int	i;
	char **ptr;
	char *str;  	

	i = 0;
	while (env[i] != NULL)
		i++;
	ptr = ft_calloc((i + 1), sizeof(char **));
	if (ptr == NULL)
		return (NULL);
	i = 0;
	while (env[i] != NULL)
	{
		str = ft_calloc((ft_strlen(env[i]) + 1), sizeof(char));
		if (str == NULL)
		{
			ft_freetab(ptr);
			return (NULL);
		}
		ft_strlcpy(str, env[i], (ft_strlen(env[i]) + 1));
		ptr[i] = str;
		i++;
	}
	return (ptr);
}

int	ft_strcpy_var(char **str, char *value, int length_name, int start)
{
	char *ptr;
	int i;
	int j;

	i = -1;
	j = -1;
	ptr = ft_calloc((ft_strlen(*str) - length_name) + ft_strlen(value) + 1, sizeof(char));
	if (ptr == NULL)
		return (1);
	while (++i < start)
		ptr[i] = (*str)[i];
	while (value[++j])
		ptr[i + j] = value[j];
	while ((*str)[i + length_name])
	{
		ptr[j + i] = (*str)[i + length_name];
		i++;
	}
	ptr[ j + i] = 0;
	free(*str);
	*str = ptr;
	return (0);
}

int ft_checkvar(char *str, char *var, int *k)
{
	int	j;

	j = 0;
	while (var[j])
	{
		if (var[j] == '=') 
		{
			*k = j + 1; 
			return (1);
		}
		if (var[j] != str[j])
			break ;
		j++;
	}
	return (0);
}

int	ft_replace_var(char **str, char **env)
{
	int					i;
	int					j;
	int					omit;

	i = 0;
	j = 0;
	omit = 0;
	while ((*str)[i] != 0)
	{
		omit = ft_omit_quote_apostrophe((*str)[i], omit, NULL, 0);
		if ((*str)[i] == '$' && omit != 2)
		{
			i++;
			while (*env != NULL)
			{
				if (ft_checkvar(&(*str)[i], *env, &j) == 1)
				{
					if (ft_strcpy_var(str, &(*env)[j], j, i - 1) == 1)
						return (1);
					else 
					{
						return (0);
					}
				}
				env = env + 1;
			}
		}
		i++;
	}
	return (0);
}


int	ft_yerror(int nb)
{
	if (nb == ERROR_PIPE)
	{
		printf("bash: syntax error near unexpected token `|'\n");
		return (258);
	}
	if (nb == ERROR_MEMORY)
	{
		printf("Error! memory not allocated");
		return (5);
	}
	if (nb == ERROR_REDIRECTION)
		return (258);
	return (1);
}

void	*ft_create_inputs(t_data *data)
{
	t_input	*ptr;
	int 		i; 	

	i = 0;
	ptr = ft_calloc((data->nb_pipe + 1), sizeof(t_input));
	if (ptr == NULL)	
		return (NULL);
	while (i < (data->nb_pipe + 1))
	{
		ptr->files = ft_calloc(1 ,sizeof(t_file));
		if (ptr->files == NULL)	
			return (NULL);
		ptr->files->type = -1; 
		i++;
	}
	data->inputs = ptr;
	return (NULL);
}

int	ft_fulling_inputs_cmds(t_data *data)
{
	int i;
	char **cmd; 
	char **ptr;

	i = 0; 
	cmd = ft_split_and_omit(data->temp, '|', 1);
	i = 0;
	while (i <= data->nb_pipe)
	{
		ptr = ft_split_and_omit(cmd[i], ' ', 0);	
		if (ptr == NULL)
			return (ft_freetab(cmd));
		data->inputs[i].cmds = ptr; 
		i++;
	}
	ft_freetab(cmd);
	return (0);
}

int	ft_if_not_cmd_after_last_pipe(t_data *data)
{
    char *ptr;	
	char **cmd;

	ptr = NULL;	
	if (data->inputs[data->nb_pipe].cmds == NULL)
	{
		ptr = readline(">");
		while (*ptr == 0)
			ptr = readline(">");
		cmd = ft_split_and_omit(ptr, ' ', 0);
		free(ptr);
		if (cmd == NULL)
			return (1);
		data->inputs[data->nb_pipe].cmds = cmd; 
	}
	return (0);
}

int	ft_yparsing(t_data *data)
{
	int		i;
	int error;

	i = 0;
	error = 0;
	data->env = ft_envcpy(data->env);
	if (data->env == NULL)
		return(ft_yerror(ERROR_MEMORY));
	data->nb_pipe =	ft_ycheck_pipe(data->temp);
	if (data->nb_pipe == -1)
		return(ft_yerror(ERROR_PIPE));
	if (ft_if_quotes_not_closes(data) == NULL)
		return(ft_yerror(ERROR_MEMORY));
	if (ft_replace_var(&data->temp, data->env) == 1)
		return(ft_yerror(ERROR_MEMORY));
	ft_create_inputs(data);
	error = ft_fulling_inputs_cmds(data);
	if (error != 0)
		return (ft_yerror(error));
	error = ft_parse_redirection(data);
	if (error != 0)
		return (ft_yerror(error));
	if (ft_if_not_cmd_after_last_pipe(data) == 1)
		return(ft_yerror(ERROR_MEMORY));
	if (data->inputs == NULL)
		return(ft_yerror(5));
	ft_yprint_input(data);
	return (0);
}
