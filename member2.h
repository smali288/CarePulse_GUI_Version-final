#define _CRT_SECURE_NO_WARNINGS
#ifndef MEMBER2_H
#define MEMBER2_H

#include "structs.h"

/* Priority Queue Management */
int initQueue(void);
int growQueue(void);
void freeQueue(void);
int enqueuePatient(int id, const char* name, int severity, int emergency);
int enqueueExistingPatient(const Patient* p);
int dequeuePatient(Patient* out);
int removeFromQueueById(int id);
void displayQueue(void);

/* 2D Ambulance Grid Management */
void initAmbulances(void);
void displayAmbulances(void);
int bookAmbulance(int vehicle, int slot);
int cancelAmbulance(int vehicle, int slot);
int countActiveAmbulances(void);

/* Simulation & Doctor Stamina */
void printStaminaBar(int stamina);
void massCasualtySimulation(int count);
void treatNextPatient(int* stamina);

#endif
