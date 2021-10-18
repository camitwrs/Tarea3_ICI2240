#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap* new = (TreeMap*) malloc(sizeof(TreeMap));
   
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    
    return new;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {
    TreeNode* aux = tree->root;

    if (aux == NULL) {
        tree->root = tree->current = createTreeNode(key, value);
        return;
    }

    while(aux != NULL) {
        if(is_equal(tree, key, aux->pair->key) == 1) {
            tree->current = aux;
            return;
        }

        if(tree->lower_than(key, aux->pair->key) == 1) {
            if(aux->left == NULL) {
                TreeNode* new = createTreeNode(key, value);
                aux->left = new;
                new->parent = aux;
            }

            aux = aux->left;
        }
        else{
            if(aux->right == NULL) {
                TreeNode* new = createTreeNode(key, value);
                aux->right = new;
                new->parent = aux;
            }
            aux = aux->right;
        }
    }

    TreeNode* new = createTreeNode(key, value);
    aux = new;
}

TreeNode * minimum(TreeNode * x){
    if (x == NULL)
        return NULL;

    TreeNode* aux = x->left;

    if (aux == NULL)
        return x;

    while(aux->left != NULL) {
        aux = aux->left;
    }

    return aux;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    if(!node->left && !node->right) {
        if(node->parent->right == node)
            node->parent->right = NULL;
        else
            node->parent->left = NULL;
    } else {
        if(!node->left && node->right) {
            if(node->parent->right == node)
                node->parent->right = node->right;
            else
                node->parent->left = node->right;

            node->right->parent = node->parent;
        } else if (node->left && !node->right) {
            if(node->parent->right == node)
                node->parent->right = node->left;
            else
                node->parent->left = node->left;

            node->left->parent = node->parent;
        } else {
            TreeNode* min = minimum(node->right);
            
            node->pair->key = min->pair->key;
            node->pair->value = min->pair->value;

            removeNode(tree, min);
        }
    }
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair * searchTreeMap(TreeMap * tree, void* key) {
    TreeNode* aux = tree->root;
    
    while (aux != NULL) {
        if(is_equal(tree, key, aux->pair->key) == 1) {
            tree->current = aux;
            return aux->pair;
        }

        if(tree->lower_than(key, aux->pair->key) == 1) 
            aux = aux->left;
        else
            aux = aux->right;
    }


    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {
    TreeNode* aux = tree->root;
    
    while (aux != NULL) {
        tree->current = aux;

        if(is_equal(tree, key, aux->pair->key) == 1) 
            return aux->pair;

        if(tree->lower_than(key, aux->pair->key) == 1) {
            if(aux->left == NULL) 
                return aux->pair;

            aux = aux->left;
        } else {
            if(aux->right == NULL) 
                return nextTreeMap(tree);

            aux = aux->right;
        }
    }


    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    TreeNode* first = minimum(tree->root);
    tree->current = first;

    return first->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    TreeNode* current = tree->current;

    if (current->right != NULL) {
        TreeNode* next = minimum(current->right);
        tree->current = next;
        return next->pair;
    }

    TreeNode* aux = tree->current->parent;

    while (aux != NULL && tree->current == aux->right) {
        tree->current = aux;
        aux = aux->parent;
    }
    
    if(aux == NULL) {
        return NULL;
    } else {
        tree->current = aux;

        return aux->pair;
    }
}