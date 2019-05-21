#include <stdio.h>
#include <assert.h>        
#include <string.h>
#include "process_list.h"

void test_is_empty() {
    process_list* list = create_process_list();
    assert(is_empty(*list));
    printf("test is_empty: ok\n");
}

void test_add() {
    process_list* list = create_process_list();
    int id;
    
    element(*list, 2);

    id = add(list, 200, "ls");
    assert(!is_empty(*list));
    assert(element(*list, id) -> pid == 200);
    assert(!strcmp(element(*list, id) -> cmd, "ls")); //strcmp return 0 when equal

    id = add(list, 201, "ps");
    assert(!is_empty(*list));
    assert(element(*list, id) -> pid == 201);
    assert(!strcmp(element(*list, id) -> cmd, "ps")); //strcmp return 0 when equal

    printf("test add: ok\n");
}

static process_list * create_basic_process_list() {
    process_list* list = create_process_list();

    add(list, 100, "ls");
    add(list, 101, "ps");
    add(list, 102, "grep");
    add(list, 103, "find");
    add(list, 104, "pwd");

    return list;
}

void test_length() {

    process_list* list = create_basic_process_list();
    assert(length(*list) == 5);
    add(list, 105, "cat");
    assert(length(*list) == 6);

    printf("test length: ok\n");
}

void test_delete() {
    process_list* list = create_basic_process_list();
    delete(list, 0);
    assert((list -> head) -> pid==101);
    delete(list, 4);
    assert(!element(*list, 4));
    delete(list, 1);
    delete(list, 2);
    delete(list, 3);
    assert(is_empty(*list));

    printf("test delete: ok\n");
}

void test_destroy() {
    process_list* list = create_basic_process_list();
    assert(list);
    destroy(list);

    printf("test destroy: ok\n");
}

void test_pidtoid() {
    process_list* list = create_basic_process_list();

    assert(pidtoid(*list, 100) == 0);
    assert(pidtoid(*list, 101) == 1);
    assert(pidtoid(*list, 102) == 2);
    assert(pidtoid(*list, 103) == 3);
    assert(pidtoid(*list, 104) == 4);
    assert(pidtoid(*list, 200) == -1);

    printf("test pidtoid: ok\n");
}

void test_changestate() {
    process_list* list = create_basic_process_list();

    assert(element(*list, 2) -> state == RUNNING);
    changestate(list, 2, SUSPENDED);
    assert(element(*list, 2) -> state == SUSPENDED);

    printf("test changestate: ok\n");
}

void test_display_element() {
    printf("test display_element :\n");

    process_list* list = create_basic_process_list();
    display_element(*element(*list, 2));

    printf("fin test display_element\n");
}

void test_display_process_list() {
    printf("test display_process_list :\n");

    process_list* list = create_basic_process_list();
    display_process_list(*list);

    printf("fin test display_process_list\n");
}

int main () {
    test_is_empty();
    test_add();
    test_length();
    test_pidtoid();
    test_changestate();
    test_delete();
    test_destroy();
    test_display_element();
    test_display_process_list();

    printf("\nAll tests: ok\n");
}
