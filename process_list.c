#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <stdio.h>

#include "process_list.h"

static cell* create_cell(process_list *l, int pid, char *cmd) {
    cell* new_cell = malloc(sizeof(cell));
    if (!new_cell) {
        return NULL;
    }
    new_cell -> id = l -> current_id;
    new_cell -> pid = pid;
    new_cell -> state = RUNNING;
    strcpy(new_cell -> cmd, cmd);
    new_cell -> next = NULL; 

    return new_cell;
}

process_list* create_process_list() {
    process_list* list = malloc(sizeof(process_list));
    if (!list) {
        return NULL;
    }
    list -> head = NULL;
    list -> current_id = 0;
    list -> length = 0;
    return list;
}

int add(process_list *l, int pid, char *cmd) {
    
    if (is_empty(*l)) {
        l -> head = create_cell(l, pid, cmd);
        if (l -> head == NULL) {
            return -1;
        }
    } else {
        cell* cursor = l -> head;
        while (cursor -> next != NULL) {
            cursor = cursor -> next;
        }
        cursor -> next = create_cell(l, pid, cmd);
        if (cursor -> next == NULL) {
            return -1;
        }
    }
    (l -> length)++;
    return (l -> current_id)++; 
}

cell * element(process_list l, int id) {
    if (!is_empty(l) && (id < l.current_id || id >= 0)) {
        cell* cursor = l.head;
        while (cursor -> next != NULL) {
            if (cursor -> id == id) {
                return cursor;
            }
            cursor = cursor -> next;
        }
        // if first or last element
        //printf("cursor != NULL : %d\n cursor -> id == id :%d\n cursor -> id = %d\n id = %d\n", cursor != NULL, cursor -> id == id, cursor -> id, id);
        if (cursor != NULL && (cursor -> id == id)) {
            return cursor;
        }
    }
    return NULL; // if not found or impossible id
}

int pidtoid(process_list l, int pid) {
    if (!is_empty(l)) {
        cell* cursor = l.head;
        while (cursor -> next != NULL) {
            if (cursor -> pid == pid) {
                return cursor -> id;
            }
            cursor = cursor -> next;
        }
        // first or last element
        if (cursor != NULL && cursor -> pid == pid) {
            return cursor -> id;
        }
    } 
    // empty list or not found
    return -1;
}

int idtopid(process_list l, int id) {
    //printf("idtopid id = %d\n", id);
    cell* cellule = element(l, id);
    if (cellule == NULL) {
        return -1;
    } else {
        return cellule -> pid;
    }
}

void changestate(process_list* l, int id, enum state_t newstate) {
    // The fact that newstate is a state_t is already checked by the langage I hope
    element(*l, id) -> state = newstate;
}

bool is_empty(process_list l) {
    if (l.head == NULL) {
        return true;
    } else {
        return false;
    }
}

int length(process_list list) {
    return list.length;
}

void delete(process_list* list, int id) {
    if (!is_empty(*list)) {
        if (length(*list) == 1) {
            if ((list -> head) -> id == id) {
                free(list -> head);
                list -> head = NULL;
                (list -> length)--;
            }
        } 
        else {
            // found at the begining
            if ((list -> head) -> id == id) {
                cell* cell_to_del = list -> head;
                list -> head = (list -> head) -> next;
                (list -> length)--;
                free(cell_to_del);
            }
            // found in the middle or in the end
            // here, the length of the list is at least 2
            else {
                cell* cursor = (list -> head) -> next; 
                cell* previous_cursor = (list -> head);
                bool found = false;
                while (cursor -> next != NULL && !found) { 
                    if (cursor -> id == id) {
                        found = true;
                        (previous_cursor -> next) = cursor -> next;
                        (list -> length)--;
                        free(cursor);
                    }
                    previous_cursor = cursor;
                    cursor = cursor -> next;
                }
                // found in the end
                if (cursor -> next == NULL && !found && (cursor -> id == id)) {
                    (previous_cursor -> next) = NULL;
                    (list -> length)--;
                    free(cursor);
                }
            }
        }
    }
}

void destroy(process_list* list) {
    cell* cursor = list -> head;
    cell* next_cursor = cursor;
    while (cursor != NULL) {
        next_cursor = cursor -> next;
        free(cursor);
        cursor = next_cursor;
    }
    free(list);
}

void display_element(cell element) {
    printf("Process id=[%d]\n", element.id);
    /* printf("id:\t%d\n", element -> id); */
    printf("    pid:\t%d\n", element.pid);
    printf("    state:\t");
    switch (element.state) {
        case SUSPENDED: printf("Suspended\n"); break;
        case RUNNING: printf("Running\n"); break;
    }
    printf("    cmd:\t%s\n", element.cmd);
}

void display_process_list(process_list list) {
    cell* cursor = list.head;
    while (cursor != NULL) {
        display_element(*cursor);
        cursor = cursor -> next;
        if (cursor != NULL) {
            printf("\n");
        }
    }
}
