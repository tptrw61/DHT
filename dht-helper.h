// eventually put code in here, e.g., list operations
#ifndef DHT_HELPER_H
#define DHT_HELPER_H

typedef struct node {
	int key, value;
	struct node *next;
} Node;

typedef struct {
	Node *head;
} List;

void insertNode(List *list, int key, int value);
Node * getNode(List *list, int key);
void removeNode(List *list, int key);

void putNode(List *list, int key, int value);

#endif
