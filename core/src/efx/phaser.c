#include "../common.h"


/**
 * Phaser structure.
 */
struct amp_phaser_t {
};


/**
 * Create a new phaser.
 *   &returns: The phaser.
 */
struct amp_phaser_t *amp_phaser_new(void)
{
	struct amp_phaser_t *phaser;

	phaser = malloc(sizeof(struct amp_phaser_t));


	return phaser;
}

/**
 * Delete a phase.
 *   @phaser: The phaser.
 */
void amp_phaser_delete(struct amp_phaser_t *phaser)
{
	free(phaser);
}
