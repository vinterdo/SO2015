
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#include <vector>

#define MAX_CUSTOMERS 100

void *customer(void *num);
void *barber(void *);

void randwait(int secs);

sem_t waitingRoom;
sem_t barberChair;
sem_t barberPillow;
sem_t seatBelt;

int allDone = 0;

int main(int argc, char *argv[]) {
    pthread_t btid;
    pthread_t tid[MAX_CUSTOMERS];
    long RandSeed;
    int i, numCustomers, numChairs;
    int Number[MAX_CUSTOMERS];

    if (argc != 4) {
	printf("Use: <Num Customers> <Num Chairs> <rand seed>\n");
	exit(-1);
    }

    // Get the command line arguments and convert them
    // into integers.
    numCustomers = atoi(argv[1]);
    numChairs = atoi(argv[2]);
    RandSeed = atol(argv[3]);

    printf("Spiacy golibroda uzywajacy semaforow zamiast budzika.\n");

    srand48(RandSeed);

    for (i=0; i<numCustomers; i++) {
	Number[i] = i;
    }

    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);

    pthread_create(&btid, NULL, barber, NULL);

    for (i=0; i<numCustomers; i++) {
	pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
    }

    for (i=0; i<numCustomers; i++) {
	pthread_join(tid[i],NULL);
    }

    allDone = 1;
    sem_post(&barberPillow);  // Jobs done
    pthread_join(btid,NULL);
}

void *customer(void *number) {
    int num = *(int *)number;

    printf("Klient %d idzie sie golic.\n", num);
    randwait(5);
    printf("Klient %d przyszedl do salonu.\n", num);

    sem_wait(&waitingRoom); // Wait for place
    printf("Klient %d wszedl do poczekalni.\n", num);
	// Wait for chair
    sem_wait(&barberChair);
	// Set place free
    sem_post(&waitingRoom);
	// Wake the barber
    printf("Klient %d budzi fryzjera.\n", num);
    sem_post(&barberPillow);

    sem_wait(&seatBelt);

    sem_post(&barberChair);
    printf("Klient %d wychodzi, wlosy zostaja.\n", num);
}

void *barber(void *junk) {
    while (!allDone) {

	printf("Zzzzzz\n");
	sem_wait(&barberPillow);

	if (!allDone) {

	    printf("Fryzjer tnie wlosy\n");
	    randwait(3);
	    printf("Fryzjer zcial wlosy.\n");
	    sem_post(&seatBelt);
	}
	else {
	    printf("Fajrant.\n");
	}
    }
}

void randwait(int secs) {
    int len = (int) ((drand48() * secs) + 1);
    sleep(len);
}
