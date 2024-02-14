#ifndef _utility_h_
#define _utility_h_

struct Node_s
{
	struct Node_s *next;
	struct Node_s *prev;
	void *data;
};

typedef struct Node_s Node;

struct Linked_List_s
{
	Node *head;
	Node *tail;
	int size;
};

typedef struct Linked_List_s Linked_List;

Node *create_Node(void *data);
Linked_List *create_Linked_List();
void add_Node(Linked_List *ll, void *data);
void remove_Node(Linked_List *ll, void *data);
void add_after(Linked_List *ll, Node *after_Node, void *data);
void sort(Linked_List *ll, int (*cmp)(void *data1, void *data2));
void swap_Nodes(Node *a, Node *b);

typedef struct Linked_List_s Queue;

Queue *create_Queue();
void enQueue(Queue *q, void *data);
void *deQueue(Queue *q);
int isEmpty(Queue *q);

#endif
