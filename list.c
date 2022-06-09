#include <stdlib.h>
#include <stdio.h>

#define false 0;
#define true 1;

#define FORMAT "Match [%u - '%c']: %s = %ld  ---- %s = %ld ---- len = %ld\n"

typedef struct _Node
{
	int startchar;
	
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

node *list_add(node *head, int _char, size_t position1, size_t position2, size_t length)
{
	if (head == NULL)
	{
		fprintf(stderr, "list.c : head == null");
		return NULL;
	}
	
	node *newnode = calloc(1, sizeof(node));
	
	if (newnode == NULL)
	{
		perror ("list.c : cannot alloc mem");
		return NULL;
	}
	
	newnode->startchar = _char;
	newnode->pos1 = position1;
	newnode->pos2 = position2;
	newnode->len = length;
	newnode->next = head->next;
	
	head->next = newnode;
	
	
	return head->next;
}

node *list_add_tail(node *head, int _char, size_t position1, size_t position2, size_t length)
{
	if (head == NULL)
	{
		fprintf(stderr, "list.c : head == null");
		return NULL;
	}
	
	node *newnode = calloc (1, sizeof(node));
	if (newnode == NULL)
	{
		perror ("list.c : cannot alloc mem");
		return NULL;
	}
	
	newnode->startchar = _char;
	newnode->pos1 = position1;
	newnode->pos2 = position2;
	newnode->len = length;
	
	node *currnode = head;
	while (currnode->next != NULL)
		currnode = currnode->next;
		
	currnode->next = newnode;
	
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
	{
		found = (node_elem->pos1 <= position1 && position1 <= node_elem->pos1 + node_elem->len)
	 			 && (node_elem->pos2 <= position2 && position2 <= node_elem->pos2 + node_elem->len);
				 
		
		
		node_elem = node_elem->next;
	}
	
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

void list_print(FILE *stream, node *head, char *file1, char *file2)
{
	node *currnode = head -> next;
	while (currnode != NULL)
	{
		fprintf(stream, FORMAT, currnode -> startchar, currnode -> startchar, file1, currnode -> pos1, file2, currnode -> pos2, currnode -> len);
		currnode = currnode->next;
	}
	
}