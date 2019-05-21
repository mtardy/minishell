#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <stdbool.h>

enum state_t {SUSPENDED, RUNNING};

typedef struct process_list process_list;

typedef struct cell cell;

struct cell {
    int id;
    int pid;
    enum state_t state;     // Suspended or Running
    char cmd[30];           // Cmd name
    cell *next;
};

struct process_list {
    int current_id;
    int length;
    cell* head;
};

/*
 * Créer une process_list vide.
 * return: pointeur sur process_list vide ou NULL si l'allocation
 *         mémoire a échouée
 */
process_list* create_process_list();

/*
 * Ajouter un processus avec son pid et son nom dans une process_list.
 * Le processus est ajouté avec un état : RUNNING
 * param:
 *   - l   : pointeur sur la process_list à laquelle ajouter le processus
 *   - pid : le pid du processus à ajouter
 *   - cmd : le nom du processus à ajouter 
 * return: l'id du processus ajouté ou -1 si l'allocation mémoire 
 *         pour un nouvelle cellule a échouée
 */
int add(process_list *l, int pid, char *cmd);

/*
 * Récupérer un élement ie. un processus dans une process_list par son id.
 * param:
 *   - l   : pointeur sur la process_list dans laquelle rechercher
 *   - id  : identifiant du processus à rechercher
 * return: l'élement ie. le processus trouvé, retourne NULL sinon
 */
cell * element(process_list l, int id);

/*
 * Récupérer l'id d'un processus à partir du pid dans une process_list.
 * param:
 *   - l   : pointeur sur la process_list dans laquelle rechercher
 *   - pid : pid du processus à rechercher
 * return: l'id du processus associé au pid fourni, -1 si introuvable
 */
int pidtoid(process_list l, int pid);

//doc TODO
int idtopid(process_list l, int id);

/*
 * Teste si une process_list est vide.
 * param:
 *   - l  : pointeur sur la process_list à tester
 * return: un booléen indiquant si la liste est vide
 */
bool is_empty(process_list l);

/*
 * Renvoie la longueur de la process_list.
 * param:
 *   - l  : pointeur sur la process_list à tester
 * return: un entier indiquant le nombre de cellule de la liste
 */
int length(process_list l);

/*
 * doc TODO
 */
void changestate(process_list* l, int id, enum state_t newstate);

/*
 * Supprime l'élement avec l'id donné de la process_list et libère la mémoire associée.
 * param:
 *   - l  : pointeur sur la process_list
 *   - id : id de l'élement à supprimer
 */
void delete(process_list* list, int id);

/*
 * Détruit la liste et libère la mémoire associée.
 * param:
 *   - l  : pointeur sur la process_list
 */
void destroy(process_list* list);

/*
 * Affiche un élement d'une process_list.
 */
void display_element(cell element);

/*
 * Affiche une process_list.
 */
void display_process_list(process_list list);

#endif
