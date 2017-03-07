#if !defined(__LPR_RBTREE_H__)

#define __LPR_RBTREE_H__

typedef struct lpr_tag_rbnode {
    struct lpr_tag_rbnode *_parent;
    struct lpr_tag_rbnode *_left;
    struct lpr_tag_rbnode *_right;
    int	_color;
    void	*key;
    void	*data;
} lpr_rbnode_t;

extern	lpr_rbnode_t lpr_rbtree_null_node;
#define	LPR_RBTREE_NULL &lpr_rbtree_null_node

typedef void (*lpr_free_func_t)(void *);
typedef int (*lpr_cmp_func_t)(void *, void *);
typedef void *(*lpr_malloc_func_t)(size_t);

typedef struct  {
    /* The root of the red-black tree */
    lpr_rbnode_t	*root;

    /* The number of the nodes in the tree */
    unsigned long count;

    /* Free and compare functions */
    lpr_malloc_func_t mallocf;
    lpr_cmp_func_t cmp;

    lpr_free_func_t freekeyf;
    lpr_free_func_t freedataf;
} lpr_rbtree_t;

#if defined(__cplusplus)
extern "C"
{
#endif
	
lpr_rbtree_t *lpr_rbtree_create(void *(*mallocf)(size_t), int (*cmpf)(void *, void *), 
						void (*freekey)(void *), void (*freedata)(void *));
void *lpr_rbtree_insert(lpr_rbtree_t *rbtree, void *key, void *data, int overwrite);
lpr_rbnode_t *lpr_rbtree_lower_bound(lpr_rbtree_t *rbtree, const void *key);
void *lpr_rbtree_search(lpr_rbtree_t *rbtree, const void *key);
void lpr_rbtree_destroy(lpr_rbtree_t *rbtree);
lpr_rbnode_t *lpr_rbtree_first(lpr_rbtree_t *rbtree);
lpr_rbnode_t *lpr_rbtree_next(lpr_rbnode_t *node);
void lpr_rbtree_print(lpr_rbnode_t *node, void (*pf)(void* key, void* data));

#if defined(__cplusplus)
}
#endif

#define	lpr_rbtree_last() LPR_RBTREE_NULL

#define	LPR_RBTREE_WALK(rbtree, iterator) \
	for(iterator = lpr_rbtree_first(rbtree); iterator != lpr_rbtree_last() ;iterator = lpr_rbtree_next(iterator))

#endif 

