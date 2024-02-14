#ifndef _utility_h_
#define _utility_h_

struct node_structure
{
	struct node_structure *prev;
	void *val;
	struct node_structure *next;
};
typedef struct node_structure Node;

struct linked_list_structure
{
	int size;
	Node *tail;
	Node *head;
};
typedef struct linked_list_structure DoublyLinkedList;

typedef struct linked_list_structure Queue;

void *pollFromQueue(Queue *q);
void addToQueue(Queue *q, void *val);
Queue *createQueue();

void swapNodes(Node *a, Node *b);
Node *create_node(void *val);
void insertAfterNode(DoublyLinkedList *ll, Node *pos, void *val);
DoublyLinkedList *createLinkedList();
void sortLinkedList(DoublyLinkedList *ll, int (*cmp)(void *value1, void *value2));
void addNode(DoublyLinkedList *ll, void *val);

#endif