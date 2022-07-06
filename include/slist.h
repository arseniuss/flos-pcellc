/*
 * Copyright (C) 2022 Armands Arseniuss Skolmeisters <arseniuss@arseniuss.id.lv>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SLIST_H
#define SLIST_H

#include <stddef.h>

struct slist_node {
    struct slist_node* next;
    struct slist_node* prev;
};

struct slist_head {
    struct slist_node* head;
    struct slist_node* tail;
};

#define INIT_SLIST_HEAD(name) \
    do {\
        (name)->head = (name)->tail = 0; \
    } while(0)

#define INIT_SLIST_NODE(name) \
    do {\
        (name)->next = (name)->prev = 0; \
    } while(0)

static __inline__ int slist_empty(struct slist_head *head) {
    return head->head == 0 && head->tail == 0;
}

static __inline__ void slist_add(struct slist_node *n, struct slist_head *head) {
    if (!head->tail) head->tail = n;
    n->next = head->head;
    n->prev = 0;
    if (head->head) head->head->prev = n;
    head->head = n;
}

static __inline__ void slist_add_tail(struct slist_node *n, struct slist_head *head) {
    if (!head->head) head->head = n;
    if (head->tail) head->tail->next = n;
    n->prev = head->tail;
    n->next = 0;
    head->tail = n;
}

static __inline__ void slist_del(struct slist_node *n, struct slist_head *head) {
    if (head->head == n)
        head->head = n->next;
    if (head->tail == n)
        head->tail = n->prev;
    if (n->prev) n->prev->next = n->next;
    if (n->next) n->next->prev = n->prev;
    n->next = n->prev = 0;
}

static __inline__ void slist_append(struct slist_head *n, struct slist_head *head) {
    if (!n->head) return;
    if (head->tail && n->head) {
        head->tail->next = n->head;
        n->head->prev = head->tail->prev;
    } else {
        head->head = n->head;
        head->tail = n->tail;
    }
}

#ifndef container_of
#define container_of(ptr, type, member) ({          \
        const typeof(((type *)0)->member)*__mptr = (ptr);    \
                     (type *)((char *)__mptr - offsetof(type, member)); })
#endif

#define slist_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define slist_first_entry(h, type, member) \
    slist_entry((h)->head, type, member)

#define slist_entry_is_last(ptr, h, member) \
    ((ptr)->member.next == (h)->tail || (ptr)->member.next == 0)

#define slist_entry_next(pos, member) \
    slist_entry((pos)->member.next, typeof(*pos), member)

#define slist_for_each_entry(__pos, __h, __member) \
    for(__pos = slist_first_entry(__h, typeof(*__pos), __member); \
        __pos != container_of(0, typeof(*__pos), __member); \
        __pos = slist_entry_next(__pos, __member))



#endif /* SLIST_H */
