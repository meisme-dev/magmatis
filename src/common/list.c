#include "list.h"
#include <stdlib.h>

ListNode *list_init(void *data) {
  ListNode *node = malloc(sizeof(ListNode));
  if (!node)
    return NULL;

  node->next = node;
  node->prev = node;
  node->data = data;
  return node;
}

void *list_add(ListNode *head, void *data) {
  ListNode *node = list_init(data);
  if (!node)
    return NULL;

  node->next = head->next;
  node->prev = head;
  head->next->prev = node;
  head->next = node;

  return node;
}

void list_remove(ListNode *head, ListNode *node) {
  ListNode *next = head->next;

  while (next != head) {
    if (next == node) {
      next->prev->next = next->next;
      next->next->prev = next->prev;
      free(node);
      return;
    }

    next = next->next;
  }
}
