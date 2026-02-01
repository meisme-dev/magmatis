#pragma once

typedef struct ListNode {
  struct ListNode *next;
  struct ListNode *prev;
  void *data;
} ListNode;

ListNode *list_init(void *data);
void *list_add(ListNode *head, void *data);
void list_remove(ListNode *head, ListNode *node);
