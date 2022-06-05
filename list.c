#include <stdlib.h>
#include <stdio.h>

#define false 0;
#define true 1;

typedef struct _Node
{
	size_t pos1;
	size_t pos2;
	size_t len;
	
	struct _Node * next;
} node;


node *list_ini ()
{
	node *head = calloc (1, sizeof (node));
	if (head == NULL)
	{
		perror("list.c : cannot alloc mem");
		return NULL;
	}
	
	return head;
}

node *list_add(node *head, size_t position1, size_t position2, size_t length)
{
	if (head == NULL)
	{
		fprintf(stderr, "list.c : head == null");
		return NULL;
	}
	
	head->next = calloc (1, sizeof(node));
	if (head->next == NULL)
	{
		perror ("list.c : cannot alloc mem");
		return NULL;
	}
	
	head->next->pos1 = position1;
	head->next->pos2 = position2;
	head->next->len = length;
	
	
	return head->next;
}

void list_clear(node *head)
{
	node *node_elem = head;
	node *node_next;
	while (node_elem != NULL)
	{
		node_next = node_elem->next;
		free(node_elem);
		node_elem = node_next;
	}
}

int list_check(node *head, size_t position1, size_t position2)
{
	if (head == NULL)
	{
		fprintf(stderr, "list.c : head == null");
		return -1;
	}
	
	node *node_elem = head->next;
	int found = false; 
	
	while (node_elem != NULL && !found)	
		found = (node_elem->pos1 <= position1 && position1 <= node_elem->pos1 + node_elem->len)
 			 && (node_elem->pos2 <= position2 && position2 <= node_elem->pos2 + node_elem->len);
	
	if (!found || node_elem == NULL)
		return 0;
	return node_elem->len;
}


node *list_concat(node *head_dest, node *head_src)
{
	node *tmp = head_dest;
	while (tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = head_src->next;
	
	return head_dest;
}