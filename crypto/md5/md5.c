/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   md5.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iomonad <iomonad@riseup.net>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 16:21:32 by iomonad           #+#    #+#             */
/*   Updated: 2019/04/10 13:27:39 by iomonad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <engine.h>
#include <crypto.h>
#include <stdlib.h>

/*
** @note Process md5 algorithm by
**       polling lazyly file descriptor
*/

static ssize_t	process_md5(const int fd, t_hashing *hash,
							const t_options *opts,
							const t_input *input)
{
	ssize_t		ret;
	ssize_t		i;
	char		chunk[4096];

	i = 0;
	init_md5(hash);
	while ((ret = read(fd, chunk, hash->clen)) == hash->clen)
	{
		if (opts->p)
			write(1, chunk, ret);
		md5_hash(hash, chunk);
		i += ret;
	}
	i += ret;
	pad_512(hash, ret, chunk, i * 8);
	md5_print(hash, input);
	return (i);
}

static ssize_t	process_md5_string(t_hashing *hash,
								   const t_options *opts,
	                               const t_input *input)
{
	ssize_t		i;
	uint64_t	len;

	init_md5(hash);
	len = ft_strlen(input->input);
	i = len;
	(void)opts;
	while (i > hash->clen)
	{
		md5_hash(hash, input->input + (len - i));
		i -= hash->clen;
	}
	pad_512(hash, i, input->input + (len - i), len * 8);
	md5_print(hash, input);
	return (i);
}

static int		post_process(const int fd)
{
	if (fd != 0)
	{
		if (close(fd) < 0)
		{
			ft_printf("Error while closing file descriptor %d.\n", fd);
			return (-1);
		}
	}
	return (0);
}

/*
** @note
*/

int				md5(const t_options *opts,
				const t_input *input)
{
	int			fd;
	t_hashing	hash;

	fd = STDIN;
	ft_bzero(&hash, sizeof(t_hash));
	if (input->method == FARG && input->input != NULL)
	{
		if ((fd = ffopen(input->input)) < 0)
			return (fd);
		process_md5(fd, &hash, opts, input);
		return (post_process(fd));
	}
	else if (input->method == STRING && input->input != NULL)
		process_md5_string(&hash, opts, input);
	return (0);
}
