/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrasser <jrasser@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:26:05 by jrasser           #+#    #+#             */
/*   Updated: 2022/05/27 20:27:42 by jrasser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
void	ft_free(t_var *var)
{
	if (var != NULL)
	{
		if (var->fd1 != -1)
			free(var->cmd1_fct);
		free(var->cmd1_args);
		free(var->cmd2_fct);
		free(var->cmd2_args);
	}
}
*/

void ft_exec_cmd1(t_data *data, int i)
{
	if (i != data->nb_pipe)
	{
		dup2(data->inputs[i].tube[1], STDOUT_FILENO);
		close(data->inputs[i].tube[0]);
	}
	if (data->inputs[i].file[0].fd != -1)
	{
		dup2(data->inputs[i].file[0].fd, STDIN_FILENO);
		if (execve(data->inputs[i].cmd_fct, data->inputs[i].cmds, data->env) == -1)
		{
			if (data->inputs[i].cmd_fct != NULL)
				ft_errputstr(strerror(errno), 0, 0, data);
		}
	}
}

void ft_pipe(t_data *data)
{
	char **cmd1_args;
	int i;
	int wstatus;

	i = 0;
	while (i <= data->nb_pipe)
	{
		cmd1_args = data->inputs[i].cmds;
		data->inputs[i].cmd_fct = ft_check_access(data->env, data->inputs[i].cmds[0]);
		ft_check_cmds(data->inputs[i].cmd_fct, cmd1_args[0]);

		/* OPEN FD */
		if (data->inputs[i].redir_input > 0)
			data->inputs[i].file[0].fd = open(data->inputs[i].file[0].name, O_RDONLY);
		// var->fd2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
		fprintf(stderr, "open fd : %d\n", data->inputs[i].file[0].fd);

		pipe(data->inputs[i].tube);
		data->inputs[i].child = fork();
		if (data->inputs[i].child == -1)
			perror("Error");
		if (data->inputs[i].child == 0)
			ft_exec_cmd1(data, i);
		else
		{
			if (i != data->nb_pipe)
				waitpid(data->inputs[i].child, &wstatus, WNOHANG);
			if (data->inputs[i].redir_input > 0)
				close(data->inputs[i].file[0].fd);
			else
			{
				//wait(NULL);
				//close(data->inputs[i].tube[0]);
				//close(data->inputs[i].tube[1]);
				//close(STDIN_FILENO);
				//close(STDOUT_FILENO);
			}
			//wait(NULL);
			dup2(data->inputs[i].tube[0], STDIN_FILENO);
			close(data->inputs[i].tube[1]);
			//dup2(data->inputs[i].tube[1], STDOUT_FILENO);
			//close(data->inputs[i].tube[0]);
			// dup2(data->inputs[i].fd, STDOUT_FILENO);
		}
		// ft_free(var);
		i++;
	}
}




/*
void ft_pipe(t_data *data, char **env)
{
	char *cmd1_fct;
	char **cmd1_args;
	char *cmd2_fct;
	char **cmd2_args;
	int i;
	int wstatus;
	int wstatus2;

	i = 0;
	while (i < data->nb_pipe)
	{
		cmd1_args = data->inputs[i].cmds;
		cmd2_args = data->inputs[i + 1].cmds;
		cmd1_fct = ft_check_access(env, data->inputs[i].cmd_fct);
		cmd2_fct = ft_check_access(env, data->inputs[i + 1].cmd_fct);
		ft_check_cmds(cmd1_fct, cmd1_args[0]);
		// var->fd1 = open(argv[1], O_RDONLY);
		// var->fd2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

		pipe(data->inputs[i].tube);
		data->inputs[i].child = fork();
		if (data->inputs[i].child == -1)
			perror("Error");
		if (data->inputs[i].child == 0)
		{
			fprintf(stderr, "2\n");
			ft_exec_cmd1(data, i, env);
		}
		else
		{
			dup2(data->inputs[i].tube[0], STDIN_FILENO);
			close(data->inputs[i].tube[1]);
			waitpid(data->inputs[i].child, &wstatus, WUNTRACED | WCONTINUED);
			// dup2(data->inputs[i].fd, STDOUT_FILENO);

			data->inputs[i].child2 = fork();
			if (data->inputs[i].child2 == -1)
				perror("Error");
			if (data->inputs[i].child2 == 0)
			{
				fprintf(stderr, "1\n");
				if (execve(cmd2_fct, cmd2_args, env) == -1)
					ft_errputstr(strerror(errno), 1, 1, data);
			}
			else
			{
				waitpid(data->inputs[i].child2, &wstatus2, WUNTRACED | WCONTINUED);
				// dup2(data->inputs[i].tube[1], STDOUT_FILENO);
				fprintf(stderr, "tout a la fin\n");
				close(data->inputs[i].tube[0]);
				//close(data->inputs[i].tube[1]);
				// waitpid(data->inputs[i + 1].child, NULL, WCONTINUED);
			}
		}
		// close(data->inputs[1].fd);
		// ft_free(var);
		i++;
	}
}
*/



/*
void ft_pipe(t_data *data, char **env)
{
	char *cmd1_fct;
	char **cmd1_args;
	char *cmd2_fct;
	char **cmd2_args;
	int i;
	int wstatus;
	int wstatus2;

	i = 0;
	while (i < data->nb_pipe)
	{
		cmd1_args = data->inputs[i].cmds;
		cmd2_args = data->inputs[i + 1].cmds;
		cmd1_fct = ft_check_access(env, data->inputs[i].cmd_fct);
		cmd2_fct = ft_check_access(env, data->inputs[i + 1].cmd_fct);
		ft_check_cmds(cmd1_fct, cmd1_args[0]);
		// var->fd1 = open(argv[1], O_RDONLY);
		// var->fd2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

		pipe(data->inputs[i].tube);
		data->inputs[i].child = fork();
		if (data->inputs[i].child == -1)
			perror("Error");
		if (data->inputs[i].child == 0)
		{
			printf("2\n");
			ft_exec_cmd1(data, i, env);
		}
		else
		{
			dup2(data->inputs[i].tube[0], STDIN_FILENO);
			close(data->inputs[i].tube[1]);
			waitpid(data->inputs[i].child, &wstatus, WUNTRACED | WCONTINUED);
			// dup2(data->inputs[i].fd, STDOUT_FILENO);
			if (WIFEXITED(wstatus))
			{
				printf("child exited, status=%d\n", WEXITSTATUS(wstatus));
			}
			else if (WIFSIGNALED(wstatus))
			{
				printf("child killed by signal %d (%s)",
					   WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
#ifdef WCOREDUMP
				if (WCOREDUMP(wstatus))
					printf(" (core dumped)");
#endif
				printf("\n");
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf("child stopped by signal %d (%s)\n",
					   WSTOPSIG(wstatus), strsignal(WSTOPSIG(wstatus)));
#ifdef WIFCONTINUED
			}
			else if (WIFCONTINUED(wstatus))
			{
				printf("child continued\n");
#endif
			}
			else
			{
				printf("status=%x\n",
					   (unsigned int)wstatus);
			}




			
			data->inputs[i].child2 = fork();
			if (data->inputs[i].child2 == -1)
				perror("Error");
			if (data->inputs[i].child2 == 0)
			{
				printf("1\n");
				if (execve(cmd2_fct, cmd2_args, env) == -1)
					ft_errputstr(strerror(errno), 1, 1, data);
			}
			else
			{
				waitpid(data->inputs[i].child2, &wstatus2, WUNTRACED | WCONTINUED);
				if (WIFEXITED(wstatus))
			{
				printf("child exited, status=%d\n", WEXITSTATUS(wstatus));
			}
			else if (WIFSIGNALED(wstatus))
			{
				printf("child killed by signal %d (%s)",
					   WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
#ifdef WCOREDUMP
				if (WCOREDUMP(wstatus))
					printf(" (core dumped)");
#endif
				printf("\n");
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf("child stopped by signal %d (%s)\n",
					   WSTOPSIG(wstatus), strsignal(WSTOPSIG(wstatus)));
#ifdef WIFCONTINUED
			}
			else if (WIFCONTINUED(wstatus))
			{
				printf("child continued\n");
#endif
			}
			else
			{
				printf("status=%x\n",
					   (unsigned int)wstatus);
			}


			
				// dup2(data->inputs[i].tube[1], STDOUT_FILENO);
				printf("tout a la fin\n");
				close(data->inputs[i].tube[0]);
				// waitpid(data->inputs[i + 1].child, NULL, WCONTINUED);
			}
		}
		// close(data->inputs[1].fd);

		// ft_free(var);
		i++;
	}
}

*/

/*
void	ft_pipe(t_data *data, char **env)
{
	char	*cmd1_fct;
	char	**cmd1_args;
	char	*cmd2_fct;
	char	**cmd2_args;
	int		i;

	i = 0;
	while (i < data->nb_pipe)
	{
		cmd1_args = data->inputs[i].cmds;
		cmd2_args = data->inputs[i + 1].cmds;
		cmd1_fct = ft_check_access(env, data->inputs[i].cmd_fct);
		cmd2_fct = ft_check_access(env, data->inputs[i + 1].cmd_fct);
		ft_check_cmds(cmd1_fct, cmd1_args[0]);
		//var->fd1 = open(argv[1], O_RDONLY);
		//var->fd2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
 
		pipe(data->inputs[i].tube);
		data->inputs[i].child = fork();
		if (data->inputs[i].child == -1)
			perror("Error");
		if (data->inputs[i].child == 0)
			ft_exec_cmd1(data, i, env);
		else
		{
			wait(NULL);
			//waitpid(data->inputs[i].child, NULL, WNOHANG);
			dup2(data->inputs[i].tube[0], STDIN_FILENO);
			close(data->inputs[i].tube[1]);
			//dup2(data->inputs[0].fd, STDOUT_FILENO);

			if (execve(cmd2_fct, cmd2_args, env) == -1)
			{
				ft_errputstr(strerror(errno), 1, 1, data);
			}

			//close(data->inputs[1].fd);
			close(data->inputs[i].tube[0]);
		}
		//ft_free(var);
		i++;
	}
}
*/
