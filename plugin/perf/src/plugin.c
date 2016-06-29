#include "common.h"


/**
 * State structure.
 */
struct web_t {
	struct avltree_t tree;
};

/**
 * File structure.
 *   @req, path, mime: The request path, share path, and mime type.
 */
struct file_t {
	const char *req, *path, *mime;
};

/*
 * local declarations
 */
static struct file_t filelist[] = {
	{ "/",           SHAREDIR "/amprt/perf/main.xhtml", "application/xhtml+xml" },
	{ "/js/gui.js",  SHAREDIR "/amprt/perf/gui.js",     "application/javascript" },
	{ "/js/plot.js", SHAREDIR "/amprt/perf/plot.js",    "application/javascript" },
	{ "/js/code.js", SHAREDIR "/amprt/perf/code.js",    "application/javascript" },
	{ "/style.css",  SHAREDIR "/amprt/perf/style.css",  "text/css" },
	{ NULL, NULL, NULL }
};

static void web_task(sys_fd_t fd, void *arg);
static bool web_handler(const char *path, struct http_args_t *args, void *arg);
static void web_watch(struct ml_env_t *env, void *arg);


/**
 * Create a new web manager.
 *   &returns: The web manager.
 */
struct web_t *web_new(void)
{
	struct web_t *web;

	web = malloc(sizeof(struct web_t));
	web->tree = avltree_init(compare_str, delete_noop);

	return web;
}

/**
 * Delete a web manager.
 *   @web: The web manager.
 */
void web_delete(struct web_t *web)
{
	avltree_destroy(&web->tree);
	free(web);
}

/**
 * Retrieve the web manager from the environment.
 *   @env: The environment.
 *   &returns: The web manager.
 */
struct web_t *perf_web_get(struct ml_env_t *env)
{
	struct ml_value_t *value;

	value = ml_env_lookup(env, "amp.perf.web");
	return value ? value->data.box.ref : NULL;
}

/**
 * Add a performance tracker.
 *   @env: The environment.
 *   @inst: The instance.
 */
void perf_web_add(struct ml_env_t *env, struct perf_inst_t *inst)
{
	struct web_t *web;

	web = perf_web_get(env);
	if(web == NULL)
		return;

	avltree_insert(&web->tree, inst->id, inst);
}

/**
 * Remove a performance tracker.
 *   @env: The environment.
 *   @inst: The instance.
 */
void perf_web_remove(struct ml_env_t *env, struct perf_inst_t *inst)
{
	struct web_t *web;

	web = perf_web_get(env);
	if(web == NULL)
		return;

	avltree_remove(&web->tree, inst->id);
}


/**
 * Plugin loading callback.
 *   @core: The core,
 */
void amp_plugin_load(struct amp_core_t *core)
{
	struct web_t *web;
	struct sys_task_t *task;

	web = web_new();

	task = sys_task_new(web_task, core);
	ml_env_add(&core->env, strdup("amp.perf.task"), ml_value_box(amp_box_ref(task), ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("amp.perf.web"), ml_value_box(amp_box_ref(web), ml_tag_copy(ml_tag_null)));
	ml_env_add(&core->env, strdup("Perf"), ml_value_eval(perf_inst_make, ml_tag_copy(ml_tag_null)));
}

/**
 * Plugin unloading callback.
 *   @core: The core.
 */
void amp_plugin_unload(struct amp_core_t *core)
{
	struct ml_value_t *value;
	struct amp_rt_t *rt;

	rt = amp_rt_get(core);
	if(rt == NULL)
		return;

	value = ml_env_lookup(core->env, "amp.perf.task");
	if(value != NULL)
		sys_task_delete(value->data.box.ref);

	value = ml_env_lookup(core->env, "amp.perf.web");
	if(value != NULL)
		web_delete(value->data.box.ref);
}


/**
 * Process the web task.
 *   @fd: The synchronization file descriptor.
 *   @arg: The argument.
 */
static void web_task(sys_fd_t fd, void *arg)
{
#define chk(err) do { if(err != NULL) { fprintf(stderr, "%s\n", err); free(err); return; } } while(0)
	struct http_server_t *server;
	struct amp_rt_t *rt;
	struct amp_core_t *core = arg;

	rt = amp_rt_get(core);
	if(rt == NULL)
		return;

	chk(http_server_open(&server, 8080));

	amp_rt_watch(rt, web_watch, NULL);

	while(true) {
		unsigned int n = http_server_poll(server, NULL) + 1;
		struct sys_poll_t poll[n];

		poll[0] = sys_poll_fd(fd, sys_poll_in_e);
		http_server_poll(server, poll+1);

		sys_poll(poll, n, -1);

		if(poll[0].revents)
			break;

		http_server_proc(server, poll+1, web_handler, perf_web_get(core->env));
	}

	http_server_close(server);
}

/**
 * Handle web requests.
 *   @path: The path.
 *   @args: The request and response arguments.
 *   @arg: The argument.
 *   &returns: True if handled, false otherwise.
 */
static bool web_handler(const char *path, struct http_args_t *args, void *arg)
{
	unsigned int i;
	struct web_t *web = arg;

	for(i = 0; filelist[i].req != NULL; i++) {
		if(strcmp(filelist[i].req, path) == 0)
			break;
	}

	if(filelist[i].req != NULL) {
		FILE *file;
		ssize_t rd;
		uint8_t buf[32*1024];

		file = fopen(filelist[i].path, "r");
		if(file == NULL)
			fatal("Missing file '%s'.", filelist[i].path);

		while(!feof(file)) {
			rd = fread(buf, 1, sizeof(buf), file);
			if((rd < 0) || ((rd == 0) && !feof(file)))
				fatal("Failure to read from '%s'.", filelist[i].path);

			io_file_write(args->file, buf, rd);
		}

		http_head_add(&args->resp, "Content-Type", filelist[i].mime);
		fclose(file);
	}
	else if(strcmp(path, "/list") == 0) {
		struct avltree_inst_t *inst;

		http_head_add(&args->resp, "Content-Type", "text/plaintext");

		for(inst = avltree_first(&web->tree); inst != NULL; inst = avltree_next(inst)) {
			struct perf_inst_t *cur = inst->val;

			hprintf(args->file, "cur: %s  .... val %.3f\n", cur->id, amp_perf_ave(&cur->amp));
		}
	}
	else if(strcmp(path, "/json") == 0) {
		struct avltree_inst_t *inst;

		http_head_add(&args->resp, "Content-Type", "application/json");

		hprintf(args->file, "{");
		for(inst = avltree_first(&web->tree); inst != NULL; inst = avltree_next(inst)) {
			struct perf_inst_t *cur = inst->val;

			hprintf(args->file, "\"%s\": %.3f", cur->id, amp_perf_ave(&cur->amp));

			if(avltree_next(inst) != NULL)
				hprintf(args->file, ",");
		}

		hprintf(args->file, "}\n");
	}
	else if(strcmp(path, "/debug") == 0) {
		http_head_add(&args->resp, "Content-Type", "text/plaintext");
		hprintf(args->file, "debug\n");
	}
	else
		return false;

	return true;
}

/**
 * Watch for changes to the core.
 *   @env; The environment.
 *   @arg: The argument.
 */
static void web_watch(struct ml_env_t *env, void *arg)
{
}
