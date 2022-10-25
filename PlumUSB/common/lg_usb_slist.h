
/**
  ******************************************************************************
  * @file          lg_usb_slist.h
  * @brief         Reusable linked list
  * @author        Li Guo
  *                1570139720@qq.com
  * @version       1.0
  * @date          2022.01.25
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; Copyright 2021 Li Guo.
  * All rights reserved.</center></h2>
  * 
  * @htmlonly 
  * <span style='font-weight: bold'>History</span> 
  * @endhtmlonly
  * 版本|作者|时间|描述
  * ----|----|----|----
  * 1.0|Li Guo|2022.01.25|创建文件
  ******************************************************************************
  */
#pragma once

#include "string.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   如果ptr的type是结构类型，则返回ptr的member地址
 * ptr 是实际使用过程中的list地址
 * type 需要转换的结构体类型
 * &((type *)0)->member)) 这句话就是找出member在结构体type中的偏移量  其实就是用0地址这个起始地址的特性 方便计算偏移量
 * 然后拿 结构体中的list减去偏移量就能拿到结构体的首地址
 * 
 */
#define usb_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * @brief   单链表结构体
 */
struct usb_slist_node {
    struct usb_slist_node *next;           /*!< 指向下一个节点的指针 */
};
typedef struct usb_slist_node usb_slist_t; /*!< 将struct usb_slist_node 换一个变量名 方便后续使用 */
/**
  * @brief            Initialize linked list
  * @pre              None
  * @param[in]        l Linked list to be initialized
  * @retval           None
  */
static inline void usb_slist_init(usb_slist_t *l)
{
    l->next = NULL;
}

/**
  * @brief            Add node in front of linked list
  * @pre              None
  * @param[in]        l Original linked list
  * @param[in]        n Node to be added
  * @retval           None
  */
static inline void usb_slist_add_head(usb_slist_t *l, usb_slist_t *n)
{
    n->next = l->next;
    l->next = n;
}
/**
  * @brief            Add a node at the end of the linked list
  * @pre              None
  * @param[in]        l Original linked list
  * @param[in]        n Node to be added
  * @retval           None
  */
static inline void usb_slist_add_tail(usb_slist_t *l, usb_slist_t *n)
{
    while (l->next)
    {
        l = l->next;
    }

    /* append the node to the tail */
    l->next = n;
    n->next = NULL;
}
/**
  * @brief            Insert a node into the linked list
  * @pre              None
  * @param[in]        l      Original linked list
  * @param[in]        next   You need to insert a new node before next
  * @param[in]        n      New node to be inserted
  * @retval           None
  */
static inline void usb_slist_insert(usb_slist_t *l, usb_slist_t *next, usb_slist_t *n)
{
    if (!next) 
    {
        usb_slist_add_tail(next, l);/*!< 如果next为空  则在next后面加上l */ //TODO: 不知道什么意思
        return;
    }

    while (l->next) 
    {
        if (l->next == next) 
        {
            l->next = n;
            n->next = next;
        }
        l = l->next;
    }
}
/**
  * @brief            Delete a node in the linked list
  * @pre              None
  * @param[in]        l Linked list to be operated
  * @param[in]        n Node to be deleted
  * @retval           Returns the linked list after deletion
  */
static inline usb_slist_t *usb_slist_remove(usb_slist_t *l, usb_slist_t *n)
{
    /*!< Find the previous node of node n to be deleted */
    while (l->next && l->next != n) 
    {
        l = l->next;
    }
    /*!< l->next==NULL 或者  l->next==n  */

    /*!< Remove node */
    if (l->next != (usb_slist_t *)0) 
    {
        /*!< l->next==n */
        l->next = l->next->next;
    }
    return l;
}

/**
  * @brief            Returns the length of the linked list
  * @pre              None
  * @param[in]        l Need to know the length of the linked list
  * @retval           Linked list length
  */
static inline unsigned int usb_slist_len(const usb_slist_t *l)
{
    unsigned int len = 0;
    const usb_slist_t *list = l->next;

    while (list != NULL) 
    {
        list = list->next;
        len++;
    }

    return len;
}
/**
  * @brief            Check whether the linked list contains a node
  * @pre              None
  * @param[in]        l Linked list to be checked
  * @param[in]        n Determine whether to include nodes
  * @retval           If it returns 0, it is included, otherwise it is not included
  */
static inline unsigned int usb_slist_contains(usb_slist_t *l, usb_slist_t *n)
{
    while (l->next) 
    {
        if (l->next == n) 
        {
            return 0;
        }
        l = l->next;
    }
    return 1;
}
/**
  * @brief            Returns the header node of the linked list
  * @pre              None
  * @param[in]        l Linked list to be operated
  * @retval           Header node pointer
  */
static inline usb_slist_t *usb_slist_head(usb_slist_t *l)
{
    return l->next;
}
/**
  * @brief            Return tail node
  * @pre              None
  * @param[in]        l Linked list to be operated
  * @retval           Tail node pointer
  */
static inline usb_slist_t *usb_slist_tail(usb_slist_t *l)
{
    while (l->next)
    {
        l = l->next;
    }
    return l;
}
/**
  * @brief            Returns the next node of the specified node
  * @pre              None
  * @param[in]        n Linked list to be operated
  * @retval           
  */
static inline usb_slist_t *usb_slist_next(usb_slist_t *n)
{
    return n->next;
}
/**
  * @brief            Judge whether the linked list is empty
  * @pre              None
  * @param[in]        l Linked list to be operated
  * @retval           If 1 is returned, it is empty, otherwise it is not empty
  */
static inline int usb_slist_isempty(usb_slist_t *l)
{
    return l->next == NULL;
}

/**
 * @brief Initialize a linked list object
 */
#define USB_SLIST_OBJECT_INIT(object) \
    {                                 \
        NULL                          \
    }

/**
 * @brief Define a linked list object
 */
#define USB_SLIST_DEFINE(slist) \
    usb_slist_t slist = { NULL }

/**
 * @brief Get the structure of a single linked list node
 * @param node Entry node
 * @param type Type of structure
 * @param member Linked list member in structure
 */
#define usb_slist_entry(node, type, member) \
    usb_container_of(node, type, member)

/**
 * usb_slist_first_entry - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define usb_slist_first_entry(ptr, type, member) \
    usb_slist_entry((ptr)->next, type, member)

/**
 * usb_slist_tail_entry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define usb_slist_tail_entry(ptr, type, member) \
    usb_slist_entry(usb_slist_tail(ptr), type, member)

/**
 * usb_slist_first_entry_or_null - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define usb_slist_first_entry_or_null(ptr, type, member) \
    (usb_slist_isempty(ptr) ? NULL : usb_slist_first_entry(ptr, type, member))

/**
 * @brief:usb_slist_for_each - Traversal single linked list
 * @pos:    the usb_slist_t *  Pointer used to traverse the linked list
 * @head:   Header node of single linked list
 */
#define usb_slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

#define usb_slist_for_each_safe(pos, next, head)    \
    for (pos = (head)->next, next = pos->next; pos; \
         pos = next, next = pos->next)

/**
 * usb_slist_for_each_entry  -   iterate over single list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your single list.
 * @member: the name of the list_struct within the struct.
 */
#define usb_slist_for_each_entry(pos, head, member)                 \
    for (pos = usb_slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (NULL);                                    \
         pos = usb_slist_entry(pos->member.next, typeof(*pos), member))

#define usb_slist_for_each_entry_safe(pos, n, head, member)          \
    for (pos = usb_slist_entry((head)->next, typeof(*pos), member),  \
        n = usb_slist_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (NULL);                                     \
         pos = n, n = usb_slist_entry(pos->member.next, typeof(*pos), member))

#ifdef __cplusplus
}
#endif


/************************ (C) COPYRIGHT 2021 Li Guo *****END OF FILE*************/