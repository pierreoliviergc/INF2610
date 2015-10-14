/*
 * semrelay.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>

#include "semrelay.h"
#include "statistics.h"
#include "multilock.h"
#include "utils.h"

void *semrelay_worker(void *ptr) {
    unsigned long i, j;
    struct experiment *exp = ptr;

    for (i = 0; i < exp->outer; i++) {
        // TODO: attendre notre tour
        
        sem_wait((sem_t*)exp->lock + exp->rank);
        for (j = 0; j < exp->inner; j++) {
            unsigned long idx = (i * exp->inner) + j;
            statistics_add_sample(exp->data, (double) idx);
        }
        // TODO: signaler le travailleur suivant
        sem_post((sem_t*)exp->lock + ((exp->rank + 1) % exp->nr_thread));
    }
    return NULL;
}

void semrelay_init(struct experiment *exp) {
    int i;

    exp->data = make_statistics();
    // TODO: allocation d'un tableau de sémaphores sem_t dans exp->lock
    sem_t* mutex = malloc (sizeof(sem_t)*exp->nr_thread);

    
    exp->lock = mutex;

    // TODO: initialisation des sémaphores
    sem_init(&mutex[0], 0, 1);

    for(i=1;i<exp->nr_thread;i++)
    {
        if((sem_init(&mutex[i], 0, 0)) == -1)
            exit(1);
    }
    return;
}

void semrelay_done(struct experiment *exp) {
    int i;

    // copie finale dans exp->stats
    statistics_copy(exp->stats, exp->data);
    free(exp->data);

    // TODO: destruction du verrou
    // TODO: liberation de la memoire du verrou
        sem_destroy(exp->lock);
        free(exp->lock);
}

