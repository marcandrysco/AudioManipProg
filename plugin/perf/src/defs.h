#ifndef DEFS_H
#define DEFS_H

/*
 * structure prototypes
 */
struct ampweb_init_t;

/**
 * Instance type enumerator.
 *   @ampweb_inst_effect_v: Effect.
 */
enum ampweb_inst_e {
	ampweb_inst_effect_v
};

/**
 * Instance structure.
 *   @id: The identifier.
 *   @box: The box.
 *   @next: The next instance.
 */
struct ampweb_inst_t {
	char *name;
	enum ampweb_inst_e type;
	struct ampweb_init_t *init;

	struct ampweb_inst_t *prev, *next;
};

#endif
