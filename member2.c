#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member2.h"
#include "member1.h"
#include "member4.h"

static int nextRandomId = 9000;

int initQueue(void)
{
    queue = (Patient*)malloc(INITIAL_QUEUE_CAP * sizeof(Patient));
    if (queue == NULL) {
        printf("\033[1;31mError: Could not allocate memory for emergency queue.\033[0m\n");
        return 0;
    }
    queueSize = 0;
    queueCapacity = INITIAL_QUEUE_CAP;
    return 1;
}

int growQueue(void)
{
    int newCap = queueCapacity * 2;
    Patient* temp = (Patient*)realloc(queue, newCap * sizeof(Patient));
    if (temp == NULL) {
        printf("\033[1;31mError: Queue is full and memory could not be expanded.\033[0m\n");
        return 0;
    }
    queue = temp;
    queueCapacity = newCap;
    printf("\033[1;36m[Dynamic Queue expanded to capacity %d]\033[0m\n", queueCapacity);
    return 1;
}

void freeQueue(void)
{
    free(queue);
    queue = NULL;
    queueSize = 0;
    queueCapacity = 0;
}

static int comesBefore(const Patient* a, const Patient* b)
{
    if (a->emergencyFlag != b->emergencyFlag) {
        return a->emergencyFlag > b->emergencyFlag;
    }
    if (a->severity != b->severity) {
        return a->severity > b->severity;
    }
    return a->arrivalOrder < b->arrivalOrder;
}

int enqueueExistingPatient(const Patient* p)
{
    int i;
    if (p == NULL) return 0;

    for (i = 0; i < queueSize; i++) {
        if (queue[i].id == p->id) {
            printf("Patient ID %d is already in the emergency queue.\n", p->id);
            return 0;
        }
    }

    if (queueSize == queueCapacity && !growQueue()) {
        return 0;
    }

    i = queueSize - 1;
    while (i >= 0 && comesBefore(p, &queue[i])) {
        queue[i + 1] = queue[i];
        i--;
    }
    queue[i + 1] = *p;
    queueSize++;
    return 1;
}

int enqueuePatient(int id, const char* name, int severity, int emergency)
{
    Patient p;
    memset(&p, 0, sizeof(Patient));

    if (severity < 1 || severity > 5) {
        printf("Invalid severity (must be 1-5).\n");
        return 0;
    }

    p.id = id;
    strncpy(p.name, name, MAX_NAME - 1);
    p.name[MAX_NAME - 1] = '\0';
    p.age = 30;
    p.gender = 'O';
    strcpy(p.phone, "00000000000");
    strcpy(p.bloodGroup, "O+");
    p.severity = severity;
    p.emergencyFlag = (emergency || severity >= 4) ? 1 : 0;
    p.arrivalOrder = ++arrivalCounter;

    return enqueueExistingPatient(&p);
}

int dequeuePatient(Patient* out)
{
    int i;
    if (queueSize == 0) {
        printf("Emergency queue is empty.\n");
        return 0;
    }
    *out = queue[0];
    for (i = 1; i < queueSize; i++) {
        queue[i - 1] = queue[i];
    }
    queueSize--;
    return 1;
}

int removeFromQueueById(int id)
{
    int i, j;
    for (i = 0; i < queueSize; i++) {
        if (queue[i].id == id) {
            for (j = i; j < queueSize - 1; j++) {
                queue[j] = queue[j + 1];
            }
            queueSize--;
            return 1;
        }
    }
    return 0;
}

void displayQueue(void)
{
    int i;
    if (queueSize == 0) {
        printf("\nEmergency queue is currently empty.\n");
        return;
    }
    printf("\n%-4s %-6s %-20s %-4s %-10s %-7s\n",
        "Pos", "ID", "Name", "Sev", "Emergency", "Arrival");
    printf("-----------------------------------------------------------\n");
    for (i = 0; i < queueSize; i++) {
        printf("%-4d %-6d %-20s %-4d %-10s %-7d\n",
            i + 1,
            queue[i].id,
            queue[i].name,
            queue[i].severity,
            queue[i].emergencyFlag ? "YES" : "No",
            queue[i].arrivalOrder);
    }
    printf("(Queue Size: %d / Capacity: %d)\n", queueSize, queueCapacity);
}

void initAmbulances(void)
{
    int v, s;
    for (v = 0; v < VEHICLES; v++) {
        for (s = 0; s < SLOTS; s++) {
            ambulance[v][s] = 0;
        }
    }
}

void displayAmbulances(void)
{
    int v, s;
    printf("\n--- 2D AMBULANCE DISPATCH GRID ([] = Free, [X] = Booked) ---\n");
    printf("         ");
    for (s = 0; s < SLOTS; s++) {
        printf("S%-3d", s + 1);
    }
    printf("\n");
    for (v = 0; v < VEHICLES; v++) {
        printf("Amb %-4d ", v + 1);
        for (s = 0; s < SLOTS; s++) {
            if (ambulance[v][s]) {
                printf("\033[1;31m[X]\033[0m ");
            }
            else {
                printf("\033[1;32m[] \033[0m ");
            }
        }
        printf("\n");
    }
}

int bookAmbulance(int vehicle, int slot)
{
    if (vehicle < 1 || vehicle > VEHICLES || slot < 1 || slot > SLOTS) {
        printf("\033[1;31mOut of range: vehicle 1-%d, slot 1-%d.\033[0m\n", VEHICLES, SLOTS);
        return 0;
    }
    if (ambulance[vehicle - 1][slot - 1] == 1) {
        printf("\033[1;33mAmbulance %d slot %d is already booked.\033[0m\n", vehicle, slot);
        return 0;
    }
    ambulance[vehicle - 1][slot - 1] = 1;
    saveAllHospitalRecords();
    printf("\033[1;32mAmbulance %d booked for slot %d.\033[0m\n", vehicle, slot);
    return 1;
}

int cancelAmbulance(int vehicle, int slot)
{
    if (vehicle < 1 || vehicle > VEHICLES || slot < 1 || slot > SLOTS) {
        printf("\033[1;31mOut of range: vehicle 1-%d, slot 1-%d.\033[0m\n", VEHICLES, SLOTS);
        return 0;
    }
    if (ambulance[vehicle - 1][slot - 1] == 0) {
        printf("That slot is not currently booked.\n");
        return 0;
    }
    ambulance[vehicle - 1][slot - 1] = 0;
    saveAllHospitalRecords();
    printf("\033[1;32mBooking cancelled.\033[0m\n");
    return 1;
}

int countActiveAmbulances(void)
{
    int v, s, active = 0;
    for (v = 0; v < VEHICLES; v++) {
        for (s = 0; s < SLOTS; s++) {
            if (ambulance[v][s] == 1) {
                active++;
            }
        }
    }
    return active;
}

void printStaminaBar(int stamina)
{
    int i;
    if (stamina < 0) stamina = 0;
    if (stamina > STAMINA_MAX) stamina = STAMINA_MAX;

    if (stamina <= 3) printf("\033[1;31m[");
    else if (stamina <= 6) printf("\033[1;33m[");
    else printf("\033[1;32m[");

    for (i = 0; i < stamina; i++) printf("|");
    for (i = stamina; i < STAMINA_MAX; i++) printf("-");
    printf("] %d/%d\033[0m", stamina, STAMINA_MAX);
}

void massCasualtySimulation(int count)
{
    int i, severity;
    char name[MAX_NAME];
    const char* bloodGroups[] = { "O+", "A+", "B+", "O-", "AB+", "A-", "B-", "AB-" };

    for (i = 0; i < count; i++) {
        severity = (rand() % 5) + 1;
        sprintf(name, "Casualty %d", nextRandomId);

        if (enqueuePatient(nextRandomId, name, severity, severity >= 4)) {
            if (patientCount < MAX_PATIENTS) {
                Patient p;
                p.id = nextRandomId;
                strcpy(p.name, name);
                p.age = 18 + (rand() % 60);
                p.gender = (rand() % 2 == 0) ? 'M' : 'F';
                sprintf(p.phone, "0300%07d", nextRandomId);
                strcpy(p.bloodGroup, bloodGroups[rand() % 8]);
                p.severity = severity;
                p.emergencyFlag = (severity >= 4) ? 1 : 0;
                p.arrivalOrder = arrivalCounter;
                patients[patientCount++] = p;
            }
        }
        nextRandomId++;
    }
    saveAllHospitalRecords();
    printf("\033[1;31m[MASS-CASUALTY EVENT] %d emergency casualties arrived and prioritized!\033[0m\n", count);
}

void treatNextPatient(int* stamina)
{
    Patient p;
    int cost;

    if (*stamina <= 0) {
        printf("\033[1;31mDoctor is exhausted! Rest is required before treating more patients.\033[0m\n");
        return;
    }

    if (!dequeuePatient(&p)) {
        return;
    }

    cost = (p.severity >= 4) ? 2 : 1;
    *stamina -= cost;
    if (*stamina < 0) *stamina = 0;

    printf("\033[1;32mTreated %s (ID %d, Severity %d).\033[0m\nDoctor Stamina: ",
        p.name, p.id, p.severity);
    printStaminaBar(*stamina);
    printf("\n");
}
