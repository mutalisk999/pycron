#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"

#define	BLACK	0
#define	RED	1
#ifndef	NULL
#define	NULL	(void *)0
#endif

lpr_rbnode_t lpr_rbtree_null_node = {LPR_RBTREE_NULL, LPR_RBTREE_NULL, LPR_RBTREE_NULL, BLACK, NULL, NULL};

/*
 * Creates a new red black tree, intializes and returns a pointer to it.
 *
 * Return NULL if mallocf() fails.
 *
 */
lpr_rbtree_t *lpr_rbtree_create(lpr_malloc_func_t mallocf, lpr_cmp_func_t cmpf, 
						lpr_free_func_t freekey, lpr_free_func_t freedata)
{
    lpr_rbtree_t *rbtree;

    /* Allocate memory for it */
    if((rbtree = (lpr_rbtree_t *)mallocf(sizeof(lpr_rbtree_t))) == NULL) {
        return NULL;
    }

    /* Initialize it */
    rbtree->root = LPR_RBTREE_NULL;
    rbtree->count = 0;
    rbtree->mallocf = mallocf;
    rbtree->cmp = cmpf;
    rbtree->freekeyf = freekey;
    rbtree->freedataf = freedata;
    return rbtree;
}

/*
 * Rotates the node to the _left.
 *
 */
static void lpr_rbtree_rotate_left(lpr_rbtree_t *rbtree, lpr_rbnode_t *node)
{
    lpr_rbnode_t *_right = node->_right;
    node->_right = _right->_left;
    if(_right->_left != LPR_RBTREE_NULL)
        _right->_left->_parent = node;

    _right->_parent = node->_parent;

    if(node->_parent != LPR_RBTREE_NULL) {
        if(node == node->_parent->_left) {
            node->_parent->_left = _right;
        } else  {
            node->_parent->_right = _right;
        }
    } else {
        rbtree->root = _right;
    }
    _right->_left = node;
    node->_parent = _right;
}

/*
 * Rotates the node to the _right.
 *
 */
static void rbtree_rotate_right(lpr_rbtree_t *rbtree,
	lpr_rbnode_t *node)
{
    lpr_rbnode_t *_left = node->_left;
    node->_left = _left->_right;
    if(_left->_right != LPR_RBTREE_NULL)
        _left->_right->_parent = node;

    _left->_parent = node->_parent;

    if(node->_parent != LPR_RBTREE_NULL) {
        if(node == node->_parent->_right) {
            node->_parent->_right = _left;
        } else  {
            node->_parent->_left = _left;
        }
    } else {
        rbtree->root = _left;
    }
    _left->_right = node;
    node->_parent = _left;
}

static void rbtree_insert_fixup(lpr_rbtree_t *rbtree, lpr_rbnode_t *node)
{
    lpr_rbnode_t	*uncle;

    /* While not at the root and need fixing... */
    while(node != rbtree->root && node->_parent->_color == RED) {
        /* If our _parent is _left child of our grandparent... */
        if(node->_parent == node->_parent->_parent->_left) {
            uncle = node->_parent->_parent->_right;

            /* If our uncle is red... */
            if(uncle->_color == RED) {
                /* Paint the _parent and the uncle black... */
                node->_parent->_color = BLACK;
                uncle->_color = BLACK;

                /* And the grandparent red... */
                node->_parent->_parent->_color = RED;

                /* And continue fixing the grandparent */
                node = node->_parent->_parent;
            } else {				/* Our uncle is black... */
                /* Are we the _right child? */
                if(node == node->_parent->_right) {
                    node = node->_parent;
                    lpr_rbtree_rotate_left(rbtree, node);
                }
                /* Now we're the _left child, repaint and rotate... */
                node->_parent->_color = BLACK;
                node->_parent->_parent->_color = RED;
                rbtree_rotate_right(rbtree, node->_parent->_parent);
            }
        } else {
            uncle = node->_parent->_parent->_left;

            /* If our uncle is red... */
            if(uncle->_color == RED) {
                /* Paint the _parent and the uncle black... */
                node->_parent->_color = BLACK;
                uncle->_color = BLACK;

                /* And the grandparent red... */
                node->_parent->_parent->_color = RED;

                /* And continue fixing the grandparent */
                node = node->_parent->_parent;
            } else {				/* Our uncle is black... */
                /* Are we the _right child? */
                if(node == node->_parent->_left) {
                    node = node->_parent;
                    rbtree_rotate_right(rbtree, node);
                }
                /* Now we're the _right child, repaint and rotate... */
                node->_parent->_color = BLACK;
                node->_parent->_parent->_color = RED;
                lpr_rbtree_rotate_left(rbtree, node->_parent->_parent);
            }
        }
    }
    rbtree->root->_color = BLACK;
}


/*
 * Inserts a node into a red black tree.
 *
 * Returns if rbtree->mallocf() fails or the pointer to the newly added
 * data otherwise.
 *
 * If told to overwrite will replace the duplicate key and data with
 * the new values (thus will NOT destroy the existing node first),
 * otherwise will return the pointer to the data of already existing
 * data.
 *
 */
void *lpr_rbtree_insert(lpr_rbtree_t *rbtree, void *key, void *data, int overwrite)
{
    /* XXX Not necessary, but keeps compiler quiet... */
    int r = 0;

    /* We start at the root of the tree */
    lpr_rbnode_t	*node = rbtree->root;
    lpr_rbnode_t	*_parent = LPR_RBTREE_NULL;

    /* Lets find the new _parent... */
    while(node != LPR_RBTREE_NULL) {
        /* Compare two keys, do we have a duplicate? */
        if((r = rbtree->cmp(key, node->key)) == 0) {
            if(overwrite) {
                if(rbtree->freekeyf)
                    rbtree->freekeyf(node->key);
                if(rbtree->freedataf)
                    rbtree->freedataf(node->data);
                node->key = key;
                node->data = data;
            }
            return node->data;
        }
        _parent = node;

        if(r < 0) {
            node = node->_left;
        } else {
            node = node->_right;
        }
    }

    /* Create the new node */
    if((node = (lpr_rbnode_t *)rbtree->mallocf(sizeof(lpr_rbnode_t))) == NULL) {
        return NULL;
    }

    node->_parent = _parent;
    node->_left = node->_right = LPR_RBTREE_NULL;
    node->_color = RED;
    node->key = key;
    node->data = data;
    rbtree->count++;

    /* Insert it into the tree... */
    if(_parent != LPR_RBTREE_NULL) {
        if(r < 0) {
            _parent->_left = node;
        } else {
            _parent->_right = node;
        }
    } else {
        rbtree->root = node;
    }

    /* Fix up the red-black properties... */
    rbtree_insert_fixup(rbtree, node);

    return node->data;
}

void lpr_rbtree_print(lpr_rbnode_t *node, void (*pf)(void* key, void* data))
{
    fprintf(stdout, "(");
    if(node->_left != LPR_RBTREE_NULL) {
        lpr_rbtree_print(node->_left, pf);
    }
    pf(node->key, node->data);
    if(node->_right != LPR_RBTREE_NULL) {
        lpr_rbtree_print(node->_right, pf);
    }
    fprintf(stdout, ")");
}

/*
 * Searches the red black tree, returns the rbnode->key >= key or NULL otherwise.
 *
 */
lpr_rbnode_t *lpr_rbtree_lower_bound(lpr_rbtree_t *rbtree, const void *key)
{
    int r;
    lpr_rbnode_t *node;

    /* We start at root... */
    node = rbtree->root;
    
       
    /* While there are children... */
    while(node != LPR_RBTREE_NULL) {
        if((r = rbtree->cmp((void *)key, node->key)) == 0) {
            return node;
        }
        if(r < 0) {
            if(node->_left == LPR_RBTREE_NULL) {
                return node;
            }    
            node = node->_left;            
        } else {
            if(node->_right == LPR_RBTREE_NULL) {
                lpr_rbnode_t * parent = node->_parent;
                lpr_rbnode_t * ch = node;
                while (parent != LPR_RBTREE_NULL && ch == parent->_right) {
                    ch = parent;
                    parent = parent->_parent;
                }
                return parent;
            }

            node = node->_right;
        }
    }
    return LPR_RBTREE_NULL;
}

/*
 * Searches the red black tree, returns the data if key is found or NULL otherwise.
 *
 */
void *lpr_rbtree_search(lpr_rbtree_t *rbtree, const void *key)
{
    int r;
    lpr_rbnode_t *node;

    /* We start at root... */
    node = rbtree->root;

    /* While there are children... */
    while(node != LPR_RBTREE_NULL) {
        if((r = rbtree->cmp((void *)key, node->key)) == 0) {
            return node->data;
        }
        if(r < 0) {
            node = node->_left;
        } else {
            node = node->_right;
        }
    }
    return NULL;
}

/*
 * Finds the first element in the red black tree
 *
 */
lpr_rbnode_t *lpr_rbtree_first(lpr_rbtree_t *rbtree)
{
    lpr_rbnode_t *node;

    for(node = rbtree->root; node->_left != LPR_RBTREE_NULL; node = node->_left)
        ;
    return node;
}

/*
 * Returns the next node...
 *
 */
lpr_rbnode_t *lpr_rbtree_next(lpr_rbnode_t *node)
{
    lpr_rbnode_t *_parent;

    if(node->_right != LPR_RBTREE_NULL) {
        /* One _right, then keep on going _left... */
        for(node = node->_right; node->_left != LPR_RBTREE_NULL; node = node->_left)
            ;
    } else {
        _parent = node->_parent;
        while(_parent != LPR_RBTREE_NULL && node == _parent->_right) {
            node = _parent;
            _parent = _parent->_parent;
        }
        node = _parent;
    }
    return node;
}

/* void rbtree_delete __P((lpr_rbtree_t *, void *, int, int)); */
void lpr_rbtree_destroy(lpr_rbtree_t *rbtree)
{
    lpr_rbnode_t *_parent;
    lpr_rbnode_t *node;

    if(rbtree == NULL)
        return;
        
    node = rbtree->root;

    while(node != LPR_RBTREE_NULL) {
        _parent = node->_parent;
        if(node->_left != LPR_RBTREE_NULL) {
            /* Go all the way to the _left... */
            node = node->_left;
        } else if(node->_right != LPR_RBTREE_NULL) {
            /* Then to the _right... */
            node = node->_right;
        } else {
            if(rbtree->freekeyf)
                rbtree->freekeyf(node->key);
            if(rbtree->freedataf)
                rbtree->freedataf(node->data);
            free(node);

            if(_parent != LPR_RBTREE_NULL) {
                if(_parent->_left == node) {
                    _parent->_left = LPR_RBTREE_NULL;
                } else {
                    _parent->_right = LPR_RBTREE_NULL;
                }
            }
            node = _parent;
        }
    }
    free(rbtree);
}
