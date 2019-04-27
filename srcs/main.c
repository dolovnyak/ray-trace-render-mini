/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbecker <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/22 18:14:31 by sbecker           #+#    #+#             */
/*   Updated: 2019/04/27 02:59:33 by sbecker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"

void	run_render(t_conf *conf, cl_mem *mem_img,
		cl_mem *mem_objects, cl_mem *mem_lights)
{
	int		err;
	size_t	global_size;

	err = clSetKernelArg(conf->cl.kernel, 0, sizeof(cl_mem), mem_img);
	err |= clSetKernelArg(conf->cl.kernel, 1, sizeof(int), &conf->mlx.width);
	err |= clSetKernelArg(conf->cl.kernel, 2, sizeof(int), &conf->mlx.height);
	err |= clSetKernelArg(conf->cl.kernel, 3, sizeof(int), &conf->objects_num);
	err |= clSetKernelArg(conf->cl.kernel, 4, sizeof(int), &conf->lights_num);
	err |= clSetKernelArg(conf->cl.kernel, 5, sizeof(t_canvas), &conf->canvas);
	err |= clSetKernelArg(conf->cl.kernel, 6, sizeof(cl_mem), mem_objects);
	err |= clSetKernelArg(conf->cl.kernel, 7, sizeof(cl_mem), mem_lights);
	if (err != 0)
		printf("set kernel arg - error\n");
	global_size = conf->mlx.width * conf->mlx.height;
	err = clEnqueueNDRangeKernel(conf->cl.queue, conf->cl.kernel, 1, NULL,
			&global_size, NULL, 0, NULL, NULL);
	if (err != 0)
		printf("NDR - error\n");
	err = clEnqueueReadBuffer(conf->cl.queue, *mem_img, CL_TRUE, 0,
			conf->mlx.size_line * conf->mlx.height,
			conf->mlx.img_data, 0, NULL, NULL);
	if (err != 0)
		printf("read buffer - error\n");
}

void	get_mem_for_render(t_conf *conf, cl_mem *mem_img,
		cl_mem *mem_objects, cl_mem *mem_lights)
{
	int	err;

	*mem_img = clCreateBuffer(conf->cl.context, CL_MEM_READ_WRITE,
			conf->mlx.size_line * conf->mlx.height, NULL, &err);
	*mem_objects = clCreateBuffer(conf->cl.context, CL_MEM_READ_ONLY,
			sizeof(t_object3d) * conf->objects_num, NULL, &err);
	err = clEnqueueWriteBuffer(conf->cl.queue, *mem_objects, CL_TRUE, 0,
			sizeof(t_object3d) * conf->objects_num,
			conf->objects, 0, NULL, NULL);
	*mem_lights = clCreateBuffer(conf->cl.context, CL_MEM_READ_ONLY,
			sizeof(t_lights) * conf->lights_num, NULL, &err);
	err = clEnqueueWriteBuffer(conf->cl.queue, *mem_lights, CL_TRUE, 0,
			sizeof(t_lights) * conf->lights_num, conf->lights, 0, NULL, NULL);
	if (err != 0)
		printf("create buffer - error\n");
}

int		refresh(t_conf *conf)
{
	cl_mem		mem_img;
	cl_mem		mem_objects;
	cl_mem		mem_lights;
	int			err;

	get_mem_for_render(conf, &mem_img, &mem_objects, &mem_lights);
	run_render(conf, &mem_img, &mem_objects, &mem_lights);
	mlx_clear_window(conf->mlx.mlx, conf->mlx.win);
	mlx_put_image_to_window(conf->mlx.mlx, conf->mlx.win,
			conf->mlx.img_ptr, 0, 0);
	return (0);
}

int		main(void)
{
	t_conf		conf;

	initialization_cl(&conf.cl);
	initialization_mlx(&conf.mlx);
	initialization_canvas(&conf.canvas);
	initialization_scene(&conf);
	refresh(&conf);
	mlx_hook(conf.mlx.win, 2, 0, &key_press, &conf);
	mlx_hook(conf.mlx.win, 17, 0, &exit_event, (void*)0);
//	mlx_loop_hook(conf.mlx.mlx, &refresh, &conf);
	mlx_loop(conf.mlx.mlx);
	return (0);
}
