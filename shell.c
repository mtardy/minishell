#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "readcmd.h"
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/errno.h>

// Pour la commande open
#include <fcntl.h>

#include "process_list.h"

/* Variable globale afin de permettre un accès dans le handler de SIGCHLD */
process_list* liste_ps; 


void interne_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr,"-minishell: cd: expected arguments\n");
    } else {
        if (chdir(args[1]) < 0) {
            fprintf(stderr,"-minishell: cd: error\n");
        }
    }
}

int interne_exit() {
    return 0;
}

void interne_jobs(process_list liste_ps) {
    display_process_list(liste_ps); 
}

void interne_stop(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr,"-minishell: stop: expected arguments\n");
    } else {
        int id;
        // Le casting ne peut pas convertir une string en int, 
        // on peut utiliser atoi ou sscanf par exemeple
        sscanf(args[1], "%d", &id);
        int pid = idtopid(*liste_ps, id);
        if (pid < 0) {
            fprintf(stderr,"-minishell: stop: idtopid error (no id found)\n");
        } else {
            if (kill(pid, SIGSTOP) < 0) {
                fprintf(stderr,"-minishell: stop: kill error\n");
            }
        }
    }
}

void interne_bg(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr,"-minishell: bg: expected arguments\n");
    } else {
        int id;
        sscanf(args[1], "%d", &id);
        int pid = idtopid(*liste_ps, id);
        if (pid < 0) {
            fprintf(stderr,"-minishell: bg: no id found (idtopid error)\n");
        } else {
            if (kill(pid, SIGCONT) < 0) {
                fprintf(stderr,"-minishell: bg: kill error\n");
            } 
        }
    }
}

void interne_fg(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr,"-minishell: fg: expected arguments\n");
    } else {
        int id;
        sscanf(args[1], "%d", &id);
        int pid = idtopid(*liste_ps, id);
        if (pid < 0) {
            fprintf(stderr,"-minishell: fg: no id found (idtopid error)\n");
        } else {
            if (kill(pid, SIGCONT) < 0) {
                fprintf(stderr,"-minishell: fg: kill error\n");
            } else {
                // ne fonctionne pas TODO
                pause();
            }
        }
    }
}



void suivi_fils (int sig) {
    int etat_fils, pid_fils;
        printf("SIGCHLD! %d\n", pid_fils);
    do {
        pid_fils = (int) waitpid(-1, &etat_fils, WNOHANG | WUNTRACED | WCONTINUED);
        //printf("Execution? : pid_fils: %d\n", pid_fils);
        //printf("err %d = ech %d\n", errno, ECHILD);
        if ((pid_fils == -1) && (errno != ECHILD)) {
            //perror("waitpid");
            exit(EXIT_FAILURE);
        } else if (pid_fils > 0) {
            if (WIFSTOPPED(etat_fils)) {
                //printf("Suspension\n");
                changestate(liste_ps, pidtoid(*liste_ps, pid_fils), SUSPENDED);
            } else if (WIFCONTINUED(etat_fils)) {
                // Ne marche pas sur MacOS, selon le standard, SIGCHLD n'est pas
                // nécessairement généré lors d'un SIGCONT sur un fils
                // Voir : https://stackoverflow.com/questions/48487935/sigchld-is-sent-on-sigcont-on-linux-but-not-on-macos
                //printf("Reprise\n");
                changestate(liste_ps, pidtoid(*liste_ps, pid_fils), RUNNING);
            } else if (WIFEXITED(etat_fils)) {
                delete(liste_ps, pidtoid(*liste_ps, pid_fils));
                printf("Exit\n");
            } else if (WIFSIGNALED(etat_fils)) {
                printf("Signal\n");
            }
        }
    } while (pid_fils > 0);
    /* autres actions après le suivi des changements d'état */
}

int main() {
    struct cmdline *cmd;
    liste_ps = create_process_list();
    int execution = 1;

    struct sigaction traitement_fils;
    traitement_fils.sa_handler = suivi_fils;
    traitement_fils.sa_flags = SA_RESTART;
    sigemptyset(&(traitement_fils.sa_mask));
    sigaction(SIGCHLD, &traitement_fils, NULL);

    while (execution) {
        printf("sh$ ");
        cmd = readcmd();
        int i = 0;
        bool pipe_exist = false;
        int p[2];
        
        if (cmd == NULL) {
            fprintf(stderr, "-minishell: readcmd() a retourné NULL\n");
            exit(EXIT_FAILURE);
        }

        // Vrai si au moins 2 cmd ont été lancées
        // || is a short circuiting operator
        
        //printf("cmd -> seq[i] != NULL && cmd -> seq[i + 1] != NULL = %d\n", cmd -> seq[i] != NULL && cmd -> seq[i + 1] != NULL);
        if (cmd -> seq[i] != NULL && cmd -> seq[i + 1] != NULL) {
            if (pipe(p) < 0) {
                fprintf(stderr, "-minishell: pipe creation fail\n");
            } else {
                pipe_exist = true;
            }
        }
        

        // Exécuter toutes les commandes de la ligne
        while(cmd->seq[i] != NULL && execution) {
            if (strcmp(cmd->seq[i][0], "cd") == 0) {
                interne_cd(cmd->seq[i]);
            } else if (strcmp(cmd->seq[i][0], "exit") == 0) {
                execution = interne_exit();
            } else if (strcmp(cmd->seq[i][0], "jobs") == 0) {
                interne_jobs(*liste_ps);
            } else if (strcmp(cmd->seq[i][0], "stop") == 0) {
                interne_stop(cmd->seq[i]);
            } else if (strcmp(cmd->seq[i][0], "bg") == 0) {
                interne_bg(cmd->seq[i]);
            } else if (strcmp(cmd->seq[i][0], "fg") == 0) {
                interne_fg(cmd->seq[i]);
            }
        
            else {
                // On crée un fils pour tenter d'exécuter le code
                pid_t pid_fils;
                pid_fils = fork();

                if (pid_fils < 0) {
                    perror("Le fork a échoué\n");
                }
                else if (pid_fils == 0) {
                    // Redirections
                    // cmd > file
                    if (cmd->out != NULL) {
                       int fdes = open(cmd->out, O_TRUNC|O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
                       if (fdes < 0) {
                           fprintf(stderr, "-minishell: erreur redirection out (open)\n");
                       }
                       if (dup2(fdes, STDOUT_FILENO) < 0) {
                           fprintf(stderr, "-minishell: erreur lors de dup2 (out)\n");
                       }
                    }
                    // cmd < file
                    if (cmd->in != NULL) {
                       int fdes = open(cmd->in, O_RDONLY);
                       if (fdes < 0) {
                           fprintf(stderr, "-minishell: erreur redirection in (open)\n");
                       }
                       if (dup2(fdes, STDIN_FILENO) < 0) {
                           fprintf(stderr, "-minishell: erreur lors de dup2 (in)\n");
                       }
                    }

                    // Gestion pipeline
                    if (pipe_exist) {
                        if (i == 0) {
                            if (close(p[0]) < 0) {
                                fprintf(stderr, "-minishell: pipeline close p[0] failed\n");
                            }
                            if (dup2(p[1], STDOUT_FILENO) < 0) {
                                fprintf(stderr, "-minishell: pipeline dup2 p[1] failed\n");
                            }
                            close(p[1]);
                        } 
                        // Deuxième commande
                        else if (i == 1) {
                            if (close(p[1]) < 0) {
                                fprintf(stderr, "-minishell: pipeline close p[1] failed\n");
                            }
                            if (dup2(p[0], STDIN_FILENO) < 0) {
                                fprintf(stderr, "-minishell: pipeline dup2 p[0] failed\n");
                            }
                            close(p[0]);
                        }
                    }

                    // Exécuter le code du fils souhaité
                    if (execvp(cmd->seq[i][0], cmd->seq[i]) < 0) {
                        fprintf(stderr, "-minishell: %s: command not found\n", cmd->seq[i][0]);
                        // le fils ne trouve pas le code lié à la commande : il se suicide
                        exit(EXIT_FAILURE);
                    }
                } 
                else {
                    int id = add(liste_ps, pid_fils, cmd -> seq[i][0]);

                    // Si lancement en tache principale
                    // Attendre la terminaison du fils pour reprendre
                    if (cmd->backgrounded == NULL && !pipe_exist) {
                        pause();
                    } else if (cmd->backgrounded == NULL && pipe_exist && i > 0) {
                        pause();
                    }
                    
                    if (cmd -> backgrounded != NULL) {
                        printf("[%d] %d\n", id, pid_fils); 
                    }
                }
            }
            // Passer à la commande suivante
            i++;
        } 
        // On ferme le pipe
        if (pipe_exist) {
            if (close(p[0]) < 0) {
                fprintf(stderr, "-minishell: erreur close p[0] fin boucle\n");
            }
            if (close(p[1]) < 0) {
                fprintf(stderr, "-minishell: erreur close p[1] fin boucle\n");
            }
        }
    }

    // On libère la mémoire avant la fin
    destroy(liste_ps);
    return EXIT_SUCCESS;
}
