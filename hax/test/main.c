#include "../hax.h"

int compare(const void *left, const void *right)
{
	if(left > right)
		return 1;
	else if(left < right)
		return -1;
	else
		return 0;
}

void print(struct avltree_node_t *node, int n, void *orig)
{
	if(node == NULL)
		return;

	print(node->left, n+1, orig);

	for(int i = 0; i < n; i++) printf("  ");
	//printf("%zu  %d     (%zd)\n", (uintptr_t)node->ref, node->bal, node->parent ? (uintptr_t)node->parent->ref : -1);
	printf("%c  %d\n", (uintptr_t)node->ref + 'A', node->bal);

	print(node->right, n+1, orig);
}

int chk(struct avltree_node_t *node, bool *suc)
{
	int left, right;

	left = node->left ? chk(node->left, suc) : 0;
	right = node->right ? chk(node->right, suc) : 0;

	if(node->left != NULL && (node->left->parent != node))
		printf("left-parent\n"),
		*suc = false;
	if(node->right != NULL && (node->right->parent != node))
		printf("right-parent\n"),
		*suc = false;
	if(right - left != node->bal)
		printf("balance\n"),
		*suc = false;

	return ((left > right) ? left : right) + 1;
}
bool chkroot(struct avltree_root_t *root)
{
	bool suc = true;

	if(root->node)
	chk(root->node, &suc);

	return suc;
}

#include <sys/time.h>
int main()
{
	struct avltree_root_t root;
	struct avltree_node_t node[26];

	struct timeval tm;
	gettimeofday(&tm, NULL);
	srand(tm.tv_sec + tm.tv_usec);

	for(int i = 0; i < 26; i++)
		node[i].ref = (void *)(intptr_t)i;

	root = avltree_root_init(compare);

	if(0) {
	avltree_root_insert(&root, &node[3]);
	avltree_root_insert(&root, &node[2]);
	avltree_root_insert(&root, &node[1]);
	avltree_root_insert(&root, &node[8]);
	avltree_root_insert(&root, &node[5]);
	avltree_root_insert(&root, &node[4]);
	print(root.node, 0, node);
	avltree_root_remove(&root, (void *)3);
	printf("==========\n");
	print(root.node, 0, node);
		if(!chkroot(&root))
			abort();

		return 0;
	}

	for(int i = 0; i < 1; i++) {
		intptr_t v = rand() % 26;

		if(avltree_root_lookup(&root, (void *)v) == NULL)
			avltree_root_insert(&root, &node[v]);
	}

	print(root.node, 0, node);
	printf("==========\n");

			avltree_root_remove(&root, root.node->ref);

	for(int i = 0; i < 400000; i++) {
		//print(root.node, 0, node);
		if(!chkroot(&root))
			abort();

		intptr_t v = rand() % 26;

		if(avltree_root_lookup(&root, (void *)v) == NULL)
			//printf("== add: %c\n", v + 'A'),
			avltree_root_insert(&root, &node[v]);
		else
			//printf("== rem: %c\n", v + 'A'),
			avltree_root_remove(&root, (void *)v);

	}

	print(root.node, 0, node);

	printf("done\n");
	return 0;
}
