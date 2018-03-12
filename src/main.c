/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgourdin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/23 17:12:38 by jgourdin          #+#    #+#             */
/*   Updated: 2018/02/28 08:18:02 by jgourdin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rtv1.h"
#include "xml_parser.h"

/*static void		events(void *args)
{
	t_env	*env;

	env = ((t_args*)args)->env;
	mlx_hook(env->win, 17, 0L, &quit, args);
	mlx_hook(env->win, KeyPress, KeyPressMask, &keypress, args);
	mlx_hook(env->win, KeyRelease, KeyReleaseMask, &keyrelease, args);
	mlx_loop_hook(env->init, &loop, args);
	mlx_loop(env->init);
}

int				main(int argc, char **argv)
{
	t_env	env;
	t_args	args;
	t_view	v;
	int		err_type;

	if ((argc == 3 && ft_strcmp("print", argv[2])) || argc < 2 || argc > 3)
		error(NULL, ERR_ARGS);
	if ((err_type = init_args(&args, &env, &v)))
		error(&args, err_type);
	if ((err_type = init_map(&args, argv[1])))
		error(&args, err_type);
	if (argc == 3)
		print_map(args.map);
	events(&args);
	return (0);
}*/

int		init_img(t_env	*env)
{
	t_img	*img;

	if (!(img = (t_img*)malloc(sizeof(t_img))))
		return (FAILURE);
	img->ptr = mlx_new_image(env->init, WIN_WIDTH, WIN_HEIGHT);
	img->endian = 0;
	img->width = WIN_WIDTH * COLOR_DEPTH / 8;
	img->height = WIN_HEIGHT;
	img->color_depth = COLOR_DEPTH / 8;
	img->data = mlx_get_data_addr(img->ptr, &img->color_depth,
		&img->width, &img->endian);
	env->img = img;
	return (SUCCESS);
}

int		init_hook(t_env *env)
{
	env->hook.up_down = 0;
	env->hook.backforw = 0;
	env->hook.left_right = 0;
	env->hook.scale = 0;
	env->hook.trans_x = 0;
	env->hook.trans_y = 0;
	env->hook.trans_z = 0;
	env->hook.rot_x = 0;
	env->hook.rot_y = 0;
	env->hook.rot_z = 0;
	return (0);
}

int		init_env(t_env *env)
{
	init_hook(env);
	env->thread_number = THREADS_NUMBER;
	env->moving = 0;
	env->aliasing = 0.5;
	env->win_height = WIN_HEIGHT;
	env->win_width = WIN_WIDTH;
	env->fov = FOVX;
	if (!(env->init = mlx_init()))
		return (FAILURE);
	if (!(env->win = mlx_new_window(env->init, WIN_WIDTH,
		WIN_HEIGHT, "RTV1")))
		return (FAILURE);
	init_loadingscreen(env);
	if (!init_img(env))
		return (FAILURE);
	return (SUCCESS);
}

void	init_fct_arr(t_args *args)
{
	args->obj_fct[0] = &sphere_intersection;
	args->obj_fct[1] = &plane_intersection;
	args->obj_fct[2] = &cylinder_intersection;
	args->obj_fct[3] = &cone_intersection;
	args->norm_fct[0] = &sphere_normal;
	args->norm_fct[1] = &plane_normal;
	args->norm_fct[2] = &cylinder_normal;
	args->norm_fct[3] = &cone_normal;
	args->rdr_fct[0] = &render_mode_0;
	args->rdr_fct[1] = &render_mode_1;
	args->rdr_fct[2] = &render_mode_2;
	args->rdr_fct[3] = &render_mode_3;
	args->rdr_fct[4] = &render_mode_4;
	args->spec_fct[PHONG - 1] = &specular_phong;
	args->text_fct[0] = &sine_wave;
	args->text_fct[1] = &sine_cosine_wave;
	args->text_fct[2] = &stripes;
	args->text_fct[3] = &checkerboard;
	args->text_fct[4] = &weight_sum_checkerboard;
	args->bump_fct[0] = &sine_wave_bump;
}

/*static void	print_hoho(t_scene *scene)
{
	printf("************ SHD *************\n");
		for (int i = 0; i<COUNT_SHD;i++)
			printf("\t\t%d -> %d \n", i, scene->shd[i]);
	printf("******** RENDER MODE *********\n");
		printf("\t\t%d \n", scene->render_mode);
	printf("******** AMB_I **************\n");
	printf("amb_i: %f %f %f\n", scene->amb_i.x, scene->amb_i.y, scene->amb_i.z);
	printf("************ CAM ************\n");
		printf("origin: %f %f %f %f\n", scene->cam.orig.x, scene->cam.orig.y, scene->cam.orig.z, scene->cam.orig.w);
		
		printf("orient: %f %f %f %f\n", scene->cam.orient.x, scene->cam.orient.y, scene->cam.orient.z, scene->cam.orient.w);
	printf("*********** OBJ **********\n");
	char *a[] = {"spere"," plane", "cylinder", "cone",};
	char *b[] = {"NO_SHD","FACING", "SHADOW", "LAMBERT", "PHONG",};
	for (t_obj_lst *obj = scene->objs; obj != NULL; obj = obj->next)
	{
		void *p;
		(void)p;
		union bi{
			t_sphere *sphere;
			t_plane	 *plane;
			t_cone  *cone;
			t_cylinder  *cylinder;
		} bin;
		printf("\t\ttype: %s\n", a[obj->content_type]);
		if (obj->content_type == 0)
			bin.sphere = (t_sphere*)obj->content;
		else if (obj->content_type == 1)
			bin.plane = (t_plane*)obj->content;
		else if (obj->content_type == 2)
			bin.cylinder = (t_cylinder*)obj->content;
		else if (obj->content_type == 3)
			bin.cone = (t_cone*)obj->content;
		if (obj->content_type == 0)
			printf("center: %f %f %f %f\nradius: %f\n", bin.sphere->center.x, bin.sphere->center.y, bin.sphere->center.z, bin.sphere->center.w, bin.sphere->radius);
		else if (obj->content_type == 1)
			printf("point: %f %f %f %f\nnormal: %f %f %f %f\n", bin.plane->p.x, bin.plane->p.y, bin.plane->p.z, bin.plane->p.w, bin.plane->normal.x, bin.plane->normal.y, bin.plane->normal.z, bin.plane->normal.w);
		else if (obj->content_type == 2)
			printf("point: %f %f %f %f\ndirection: %f %f %f %f\nradius: %f\n", bin.cylinder->p.x, bin.cylinder->p.y, bin.cylinder->p.z, bin.cylinder->p.w, bin.cylinder->dir.x, bin.cylinder->dir.y, bin.cylinder->dir.z, bin.cylinder->dir.w, bin.cylinder->radius);
		else if (obj->content_type == 3)
			printf("point: %f %f %f %f\ndirection: %f %f %f %f\nangle: %f\nangle_tan: %f\n", bin.cone->p.x, bin.cone->p.y, bin.cone->p.z, bin.cone->p.w, bin.cone->dir.x, bin.cone->dir.y, bin.cone->dir.z, bin.cone->dir.w, bin.cone->angle, bin.cone->ang_tan);
		printf("model: %s\n", b[obj->material.model]);
		printf("amb: %f %f %f\n", obj->material.amb.x,obj->material.amb.y,obj->material.amb.z);
		printf("diff: %f %f %f\n", obj->material.diff.x,obj->material.diff.y,obj->material.diff.z);
		printf("spec: %f %f %f\n", obj->material.spec.x,obj->material.spec.y,obj->material.spec.z);
		printf("shin: %f\n", obj->material.shin);
		if (obj->next != NULL)
			printf("**************************\n");
	}
	printf("*********** LIGHT *************\n");
	char *c[] = {"DIRECTIONNAL","POINT",};
	for(t_list *lst = scene->light; lst != NULL; lst = lst->next)
	{
		(void)c;
		t_light *lgt = (t_light*)lst->content;
		printf("type: %s\n", c[lgt->type]);
		printf("type_num: %d\n", lgt->type);
		printf("vec: %f %f %f %f\n", lgt->vec.x,lgt->vec.y,lgt->vec.z,lgt->vec.w);
		printf("diff_i: %f %f %f \n", lgt->diff_i.x,lgt->diff_i.y,lgt->diff_i.z);
		printf("spec_i: %f %f %f \n", lgt->spec_i.x,lgt->spec_i.y,lgt->spec_i.z);
		printf("range: %f\n", lgt->range);
		printf("atten: %f %f %f \n", lgt->atten.x,lgt->atten.y,lgt->atten.z);
		if (lst->next != NULL)
			printf("********************\n");
	}
	printf("********************************\n");
}
*/

t_mtx4	get_camera_to_world(t_view *view)
{
	t_vec3	orient;
	t_mtx4	translation;
	t_mtx4	rotation;

	orient = init_vec3(RAD(view->orient.x),
		RAD(view->orient.y), RAD(view->orient.z));
	translation = translate(view->orig.x, view->orig.y, view->orig.z);
	rotation = quat_to_mtx(euler_to_quat(orient));
	return (mtx4_mult(translation, rotation));
}

int		init_args(t_args *args, t_env *env, t_scene *scene, char *path)
{
	(void)path;
	init_env(env);
	scene->objs = NULL;
	scene->light = NULL;
	if (!xml_parse(path, scene))
		exit(-1);
	//print_hoho(scene);
	init_fct_arr(args);
	args->env = env;
	args->scene = scene;
//	mlx_put_image_to_window(args->env->init, args->env->win, args->env->lodscreen->ptr, 0, 0);
	args->pix_buf = init_pix_buffer(env, get_camera_to_world(&scene->cam));
	return (SUCCESS);
}

void	benchmark(int res, char *def)
{
	static struct timespec start;
	struct timespec finish;
	double elapsed;

	if (!res)
		clock_gettime(CLOCK_MONOTONIC, &start);
	else
	{
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		printf("%s : %f\n", def, elapsed);
	}
}

void	benchmark_total(int res, char *def)
{
	static struct timespec start;
	struct timespec finish;
	double elapsed;

	if (!res)
		clock_gettime(CLOCK_MONOTONIC, &start);
	else
	{
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		printf("%s : %f\n", def, elapsed);
	}
}

int		get_coord(int button, int x, int y, void *args)
{
	if (button == 1)
		printf("%d, %d\n", x, y);
	args = NULL;
	return (1);
}

void	add_texture(t_scene *scene)
{
	t_obj_lst		*tmp;
	unsigned int	id;

	tmp = scene->objs;
	id = 0;
	while (tmp)
	{
		tmp->material.bump_text = NO_BUMP;
		tmp->material.bump_angle = -60;
		tmp->material.bump_scale = 0.25;
		// else if (tmp->id == 4)
		tmp->material.absorb = init_vec3(0, 0, 0);
		tmp = tmp->next;
	}
	
}

void	ft_loop(t_env env)
{
		mlx_loop(env.init);
}

int		mainsuite(/*int argc, char **argv*/)
{
	t_args	args;
	t_env	env;
	t_scene scene;
	t_obj_lst *objs;

	scene.path = "scene/04_sphere_move.xml";
	benchmark_total(0, NULL);
	// if (argc != 2)
	// {
	// 	ft_putstr("Usage: ./rtv1 docname\n");
	// 	return (0);
	// }
	benchmark(0, NULL);
	init_args(&args, &env, &scene, scene.path);
	objs = scene.objs;
	// add_texture(&scene);
	benchmark(1, "init time");
	benchmark(0, NULL);
	manage_threads(&args);
	//screenshot(env.img);
	// if (manage_threads(&args) == FAILURE)
		// trace_primary_rays(&args);
	benchmark(1, "graphics calc time");
	benchmark(0, NULL);
	mlx_put_image_to_window(env.init, env.win, env.img->ptr, 0, 0);
	benchmark(1, "display time");
	benchmark_total(1, "total time");
	mlx_hook(env.win, 17, 0L, &quit, &args);
	mlx_mouse_hook(env.win, select_obj, &args);
//	mlx_hook(env.win, KEY_PRESS, KEY_PRESS_MASK, &keypress, &args);
	mlx_hook(env.win, 2, 0, hook, &args);
//	mlx_mouse_hook(env.win, &get_coord, (void*)&args);
	mlx_loop(env.init);
	return (0);
}

int		mainfileselect(char *file)
{
	t_args	args;
	t_env	env;
	t_scene scene;
	t_obj_lst *objs;

	file = ft_strstr(file, "scene");
	// printf("%s\n", file);
	scene.path = file;
	// scene.path = "scene/04_sphere_move.xml";
	// scene.path = "file:///Users/achambon/Desktop/42Projects/testUIRT/scene/04_sphere_move.xml";
	benchmark_total(0, NULL);
	// if (argc != 2)
	// {
	// 	ft_putstr("Usage: ./rtv1 docname\n");
	// 	return (0);
	// }
	benchmark(0, NULL);
	init_args(&args, &env, &scene, scene.path);
	objs = scene.objs;
	// add_texture(&scene);
	benchmark(1, "init time");
	benchmark(0, NULL);
	manage_threads(&args);
	//screenshot(env.img);
	// if (manage_threads(&args) == FAILURE)
		// trace_primary_rays(&args);
	benchmark(1, "graphics calc time");
	benchmark(0, NULL);
	mlx_put_image_to_window(env.init, env.win, env.img->ptr, 0, 0);
	benchmark(1, "display time");
	benchmark_total(1, "total time");
	mlx_hook(env.win, 17, 0L, &quit, &args);
	mlx_mouse_hook(env.win, select_obj, &args);
//	mlx_hook(env.win, KEY_PRESS, KEY_PRESS_MASK, &keypress, &args);
	mlx_hook(env.win, 2, 0, hook, &args);
//	mlx_mouse_hook(env.win, &get_coord, (void*)&args);
	mlx_loop(env.init);
	return (0);
}

gchar *file_selected(GtkFileChooser *file_btn)
{
    gchar *file_name;
    GtkBuilder  *builder;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    gtk_builder_connect_signals(builder, NULL);
    file_name = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(file_btn));
    g_object_unref(builder); 
    printf("%s\n", (char*)file_name);
    mainfileselect((char*)file_name);
    return (file_name);
}

int		main(int argc, char **argv)
{
//////////////

	GtkBuilder      *builder; 
    GtkWidget       *window;
   	if (argc != 2)
	{
		ft_putstr("Usage: ./rtv1 docname\n");
		return (0);
	}
    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_menu"));
    gtk_builder_connect_signals(builder, NULL);
 
    g_object_unref(builder);
 
    gtk_widget_show(window);                
    gtk_main();

/*main suite ici*/
	return (0);
}

void on_window_main_destroy()
{
    gtk_main_quit();
}

void add_settingsmenu()
{
	GtkBuilder      *builder; 
    GtkWidget       *win;
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
 
    win = GTK_WIDGET(gtk_builder_get_object(builder, "settings_menu"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder); 
    gtk_widget_show(win);                
    // gtk_main();
    
}


void add_scenemenu()
{
    GtkBuilder      *builder; 
    GtkWidget       *win;
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    win = GTK_WIDGET(gtk_builder_get_object(builder, "scene_menu"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(win);                
    // gtk_main();
}

void add_objectmenu()
{
    GtkBuilder      *builder; 
    GtkWidget       *win;
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    win = GTK_WIDGET(gtk_builder_get_object(builder, "object_menu"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder); 
    gtk_widget_show(win);                
    // gtk_main();
 
}

// void    uistart_rt()
// {
//     system(rt_andscene);
// }

void    wheight_changed(GtkEntry *uiw_height)
{
    GtkBuilder  *builder;
    const gchar *wheight;
    t_uitest uitest;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    gtk_builder_connect_signals(builder, "uiw_height");
    wheight = gtk_entry_get_text(GTK_ENTRY(uiw_height));
    uitest.hauteur = atoi(wheight);
    printf("%s\n", wheight);
    printf("%d\n", uitest.hauteur);
    g_object_unref(builder); 
}

void wwidth_changed(GtkEntry *uiw_width)
{
    GtkBuilder  *builder;
    const gchar *wwidth;
    t_uitest uitest;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    gtk_builder_connect_signals(builder, "uiw_width");
    wwidth = gtk_entry_get_text(GTK_ENTRY(uiw_width));
    uitest.largeur = atoi(wwidth);
    printf("%s\n", wwidth);
    printf("%d\n", uitest.largeur);
    g_object_unref(builder); 
}

void pxlval_changed(GtkEntry *uipxl_val)
{
    GtkBuilder  *builder;
    const gchar *pxlval;
    t_uitest uitest;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    gtk_builder_connect_signals(builder, "uipxl_val");
    pxlval = gtk_entry_get_text(GTK_ENTRY(uipxl_val));
    uitest.pixilation = atoi(pxlval);
    printf("%s\n", pxlval);
    printf("%d\n", uitest.pixilation);
    g_object_unref(builder); 
}

void apply_settings(t_uitest *uitest)
{
    GtkBuilder  *builder;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "RT_menu.glade", NULL);
    gtk_builder_connect_signals(builder, "appliquer_settings");
    uitest->hauteur = 
    printf("%d\n", uitest->hauteur);
    // printf("%d\n", uitest.largeur);
    // printf("%d\n", uitest.pixilation);
    g_object_unref(builder);    
}