#include <stdio.h>
#include "../include/stack_alloc.h"

typedef struct node{
	int val;
	struct node *next;
}node_t;

node_t *create_node(int val){
	node_t *newnode = heap_alloc(sizeof(node_t));
	newnode->val = val;
	newnode->next = NULL;
}

void insert_node(node_t *head, node_t *inode){
	node_t *tmp = head;
	while(tmp->next){
		tmp = tmp->next;
	}
	tmp->next = inode;
}

void inser_node_head(node_t **head, node_t *inode){
	inode->next = *head;
	*head = inode;
}



int main(){
	node_t *head = NULL;
	char *c = heap_alloc(200);
	head = create_node(10);
	printf("value : %d",head->val);
	
}

