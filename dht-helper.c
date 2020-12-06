// eventually put code in here, e.g., list operations
#include "dht-helper.h"
#include <stdlib.h>

void insertNode(List *list, int key, int value) {
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->key = key;
	newNode->value = value;
	newNode->next = NULL;
	if (list->head == NULL) {
		list->head = newNode;
		return;
	}
	if (key < list->head->key) {
		newNode->next = list->head;
		list->head = newNode;
		return;
	}
	Node *cur = list->head;
	while (cur->next != NULL) {
		if (key < cur->next->key) {
			newNode->next = cur->next;
			cur->next = newNode;
			return;
		}
		cur = cur->next;
	}
	cur->next = newNode;
}

Node * getNode(List *list, int key) {
	if (list->head == NULL)
		return NULL;
	Node *cur = list->head;
	while (cur != NULL) {
		if (cur->key == key) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

void removeNode(List *list, int key) {
	Node *toRemove = getNode(list, key);
	if (toRemove == NULL)
		return;
	if (list->head == toRemove) {
		list->head = toRemove->next;
		free(toRemove);
		return;
	}
	Node *cur = list->head;
	while (cur->next != toRemove) {
		cur = cur->next;
	}
	cur->next = toRemove->next;
	free(toRemove);
}

void putNode(List *list, int key, int value) {
	Node *node = getNode(list, key);
	if (node == NULL) {
		insertNode(list, key, value);
	} else {
		node->value = value;
	}
}
