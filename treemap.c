#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

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
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
}


void insertTreeMap(TreeMap * tree, void* key, void * value){
    if(tree == NULL || key == NULL || value == NULL) return;
    TreeNode * aux = tree->root;
    TreeNode * parent = NULL;
    while(aux != NULL){
        parent = aux;
        if(is_equal(tree,aux->pair->key,key)) return;
        if(tree->lower_than(aux->pair->key,key)) aux = aux->right;
        else aux = aux->left;
    }
    TreeNode * new = createTreeNode(key,value);
    new->parent = parent;
    if(parent == NULL) tree->root = new;
    else{
        if(tree->lower_than(parent->pair->key,key)) parent->right = new;
        else{
            parent->left = new;
        }
    }
    tree->current = new;
}


TreeNode * minimum(TreeNode * x){
    while(x->left != NULL) x = x->left;
    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    if (tree == NULL || node == NULL) return;
    if(node->left == NULL && node->right == NULL){
        if(node->parent == NULL){
            tree->root = NULL;
        }
        else if(tree->lower_than(node->parent->pair->key,node->pair->key)){
            node->parent->right = NULL;
        }
        else
            node->parent->left = NULL;
        free(node);
    }
    else{
        if(node->right != NULL && node->left!=NULL){
            TreeNode * min = minimum(node->right);
            node->pair->key = min->pair->key;
            node->pair->value = min->pair->value;
            removeNode(tree,min);
        }
        else{
            if(node->right){
                if(node->parent == NULL)
                    tree->root = node->right;
                else if(tree->lower_than(node->parent->pair->key,node->pair->key))
                    node->parent->right = node->right;
                else
                    node->parent->left = node->right;
                node->right->parent = node->parent;
            }
            else{
                if(node->parent==NULL)
                    tree->root = node ->left;
                else if(tree->lower_than(node->parent->pair->key,node->pair->key))
                    node->parent->right = node->left;
                else
                    node->parent->left = node->left;
                node->left->parent = node->parent;
            }
            free(node);
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
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * aux = tree->root;
    while (aux != NULL){
        if (is_equal(tree, key, aux->pair->key)){
            tree->current = aux;
            return aux->pair;
        }
        else if(tree->lower_than(key, aux->pair->key))
            aux=aux->left;
        else
            aux = aux-> right;
    }
    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * aux = tree->root;
    TreeNode * ub_node = NULL;
    while (aux != NULL){
        if (is_equal(tree, key, aux->pair->key)){
            tree->current = aux;
            return aux->pair;
        }
        else if(tree->lower_than(key, aux->pair->key)){
            ub_node=aux;
            aux=aux->left;
        }
        else
            aux = aux-> right;
    }
    return ub_node?ub_node->pair:NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    TreeNode *aux=tree->root;
    if(aux==NULL) return NULL;
    while(aux->left!=NULL) aux=aux->left;
    tree->current=aux;
    return aux->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    TreeNode *aux=tree->current;
    if(aux==NULL || (aux->parent == NULL && aux->right == NULL)) return NULL;
    if(aux->right!=NULL)
        tree->current = minimum(aux->right);
    else{
        if(aux->parent&&tree->lower_than(aux->pair->key,aux->parent->pair->key))
            tree->current = aux->parent;
        else{
            while(aux->parent && tree->lower_than(aux->parent->pair->key,aux->pair->key))
                aux=aux->parent;
            tree->current = aux->parent;
        }
    }
    if(tree->current)return tree->current->pair;
    return NULL;
}
