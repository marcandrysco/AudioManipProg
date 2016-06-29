#ifndef EFX_PHASER_H
#define EFX_PHASER_H

/*
 * phaser declarations
 */
struct amp_phaser_t;

struct amp_phaser_t *amp_phaser_new(void);
void amp_phaser_delete(struct amp_phaser_t *phaser);

#endif
