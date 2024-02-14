#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

Node *createNode(void *val)
{
	Node *node = (Node *)malloc(sizeof(Node));
	node->prev = NULL;
	node->next = NULL;
	node->val = val;
	return node;
}

void addNode(DoublyLinkedList *list, void *val)
{
	Node *node = createNode(val);
	if (list->size == 0)
	{
		list->head = node;
		list->tail = node;
		list->size = 1;
	}
	else
	{
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
		list->size++;
	}
}

void swapNodes(Node *a, Node *b)
{
	void *temp = a->val;
	a->val = b->val;
	b->val = temp;
}

DoublyLinkedList *createLinkedList()
{
	DoublyLinkedList *dll = (DoublyLinkedList *)malloc(sizeof(DoublyLinkedList));
	dll->size = 0;
	dll->tail = NULL;
	dll->head = NULL;
	return dll;
}

void remove_value(DoublyLinkedList *list, void *val)
{
	Node *trav = list->head;

	while (trav != NULL && trav->val != val)
	{
		trav = trav->next;
	}

	if (trav != NULL)
	{
		if (trav->prev != NULL)
			trav->prev->next = trav->next;
		if (trav->next != NULL)
			trav->next->prev = trav->prev;
		if (list->head == trav)
			list->head = trav->next;
		if (list->tail == trav)
			list->tail = trav->prev;
		list->size--;
		free(trav);
	}
}

void remove_head(DoublyLinkedList *list)
{
	Node *trav = list->head;
	if (trav != NULL)
	{
		list->head = trav->next;
		if (list->tail == trav)
			list->tail = trav->prev;
		list->size--;
		free(trav);
	}
}

void insertAfterNode(DoublyLinkedList *list, Node *pos, void *val)
{
	Node *node = createNode(val);

	Node *next = pos->next;
	node->next = next;
	if (next != NULL)
		next->prev = node;

	node->prev = pos;
	pos->next = node;

	if (list->tail == pos)
		list->tail = node;

	list->size++;
}

void sortLinkedList(DoublyLinkedList *list, int (*cmp)(void *value1, void *value2))
{
	Node *i = list->head;
	while (i != NULL)
	{
		Node *j = i->next;
		while (j != NULL)
		{
			void *p1 = i->val;
			void *p2 = j->val;
			if ((*cmp)(p1, p2) > 0)
				swapNodes(i, j);
			j = j->next;
		}
		i = i->next;
	}
}

void addToQueue(Queue *q, void *val)
{
	Node *node = createNode(val);

	node->prev = q->tail;
	if (q->tail != NULL)
	{
		q->tail->next = node;
		q->tail = node;
	}
	else
	{
		q->tail = node;
		q->head = node;
	}
	q->size++;
}

void *pollFromQueue(Queue *q)
{
	if (q->head != NULL)
	{
		Node *trav = q->head;
		void *val = trav->val;

		Node *next = q->head->next;

		if (next != NULL)
			next->prev = NULL;
		q->head = next;

		if (q->tail == trav)
		{
			q->tail = NULL;
		}

		q->size--;
		free(trav);
		return val;
	}
}

Queue *createQueue()
{
	return createLinkedList();
}