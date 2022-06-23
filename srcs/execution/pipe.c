/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrasser <jrasser@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:26:05 by jrasser           #+#    #+#             */
/*   Updated: 2022/06/21 02:36:54 by jrasser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_dup2(t_data *data, int i)
{
	int	j;
	int	type;

	j = 0;
	while (data->inputs[i].file[j].type != 0)
	{
		type = data->inputs[i].file[j].type;
		if (type == HEREDOC && data->inputs[i].file[j].fd != -1)
		{
			close(data->inputs[i].file[j].fd);
			data->inputs[i].file[j].fd = open(data->inputs[i].file[j].name, \
			O_RDONLY);
			type = IN;
		}
		if (type == IN && data->inputs[i].file[j].fd != -1)
			dup2(data->inputs[i].file[j].fd, STDIN_FILENO);
		if (type == OUT && data->inputs[i].file[j].fd != -1)
			dup2(data->inputs[i].file[j].fd, STDOUT_FILENO);
		if (type == APPEND && data->inputs[i].file[j].fd != -1)
			dup2(data->inputs[i].file[j].fd, STDOUT_FILENO);
		j++;
	}
}

void	ft_exec_cmd(t_data *data, int i)
{
	if (i != data->nb_pipe)
	{
		dup2(data->inputs[i].tube[1], STDOUT_FILENO);
		close(data->inputs[i].tube[0]);
	}
	ft_dup2(data, i);
	if (ft_is_builtin(data, i))
	{
		if (ft_exec_builtin(data, i))
		{
			tcsetattr(0, 0, &data->termios_save);
			kill(0, SIGKILL);
		}
		if (!ft_no_need_child(data, i))
			exit(g_error_code);
	}
	else if (data->inputs[0].cmds[0])
	{
		if (execve(data->inputs[i].cmd_fct, data->inputs[i].cmds, data->env)
			== -1)
		{
			if (data->inputs[i].cmd_fct != NULL)
			{
				ft_errputstr(strerror(errno), 0, 0, NULL);
				g_error_code = errno;
			}
		}
	}
}

void	ft_fork(t_data *data, int i)
{
	int	wstatus;
	int	ret;
	int	j;

	data->inputs[i].child = fork();
	if (data->inputs[i].child == -1)
		perror("Error");
	if (data->inputs[i].child == 0)
		ft_exec_cmd(data, i);
	else
	{
		j = 0;
		while (j < 10000000)
			j++;
		ret = waitpid(data->inputs[i].child, &wstatus, WNOHANG);
		if (WIFEXITED(wstatus))
			g_error_code = WEXITSTATUS(wstatus);
		else if (WIFSIGNALED(wstatus))
			g_error_code = WTERMSIG(wstatus);
		else
			g_error_code = 0;
		while (ret < 0)
			ret = waitpid(data->inputs[i].child, &wstatus, WNOHANG);
	}
}

void	ft_close_and_free(t_data *data, int *i)
{
	dup2(data->inputs[*i].tube[0], STDIN_FILENO);
	close(data->inputs[*i].tube[1]);
	close(data->inputs[*i].tube[0]);
	ft_close_redir(data, *i);
	ft_free_section(data, *i);
	*i += 1;
}

void	ft_exec_parse(t_data *data)
{
	int	i;

	i = 0;
	while (i <= data->nb_pipe)
	{
		data->inputs[i].cmd_fct = ft_check_access(data, i);
		ft_check_redir(data, i);
		if (ft_check_fds(data, i) || ft_check_cmds(data, i))
		{
			if (data->inputs[i].cmds)
				ft_free_section(data, i);
			else
				free(data->inputs[i].file);
			i++;
			break ;
		}
		pipe(data->inputs[i].tube);
		if (ft_no_need_child(data, i))
			ft_exec_cmd(data, i);
		else
			ft_fork(data, i);
		ft_close_and_free(data, &i);
	}
	if (data->done == 0)
		free(data->inputs);
}
