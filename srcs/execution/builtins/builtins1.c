/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrasser <jrasser@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/28 17:24:18 by jrasser           #+#    #+#             */
/*   Updated: 2022/06/11 22:27:18 by jrasser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_echo(t_data *data, int i)
{
	int	j;
	int	newline;

	newline = 1;
	j = 1;
	while (data->inputs[i].cmds[j])
	{
		if (ft_strlen(data->inputs[i].cmds[j]) != 2
		|| strncmp(data->inputs[i].cmds[j], "-n", 2) != 0)
		{
			write(1, data->inputs[i].cmds[j], ft_strlen(data->inputs[i].cmds[j]));
			if (data->inputs[i].cmds[j + 1])
				write(1, " ", 1);
		}
		else
			newline = 0;
		j++;
	}
	if (newline)
		write(1, "\n", 1);
}

void	ft_cd(t_data *data, int i)
{
	chdir(data->inputs[i].cmds[1]);
}

void	ft_pwd(t_data *data, int i)
{
	char	str[500];
	
	(void)data;
	(void)i;
	getcwd(str, 500);
	write(1, str, ft_strlen(str));
	write(1, "\n", 1);
}

void	ft_exit(t_data *data, int i)
{
	(void)data;
	(void)i;
	data->done = 1;
}