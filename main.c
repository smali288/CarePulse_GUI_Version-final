#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "validation.h"
#include "security.h"
#include "menus.h"
#include "member2.h"
#include "member3.h"
#include "member4.h"

/* Shared Global Variable Definitions */
Patient patients[MAX_PATIENTS] = { 0 };
int patientCount = 0;
int arrivalCounter = 0;

/* Member 2 Globals */
Patient* queue = NULL;
int queueSize = 0;
int queueCapacity = 0;
int ambulance[VEHICLES][SLOTS] = { 0 };
int doctorStamina = STAMINA_MAX;

/* Member 3 Globals */
Medicine* medicines = NULL;
int medicineCount = 0;
int medicineCapacity = 0;

/* Member 4 Globals */
float totalRevenue = 0.0f;
int paidBills = 0;
int unpaidBills = 0;
float fundBalance = 1000000.0f;

int main(void)
{
    char currentUser[MAX_NAME] = "Guest";
    char currentRole[20] = "Staff";

    srand((unsigned int)time(NULL));
    enableConsoleColors();

    if (!initQueue() || !initializeInventory(INITIAL_MED_CAP)) {
        printf("Critical startup memory error.\n");
        return 1;
    }
    initAmbulances();

    /* Load saved records from disk before login */
    loadAllHospitalRecords();
    pauseAndClear();

    /* LOGIN GATE: Main Menu will NOT open unless user logs in */
    if (!userLoginPortal(currentUser, currentRole)) {
        freeQueue();
        freeMemoryOnExit();
        printf("Exited before login.\n");
        return 0;
    }

    /* Launch main application loop */
    mainMenu();

    /* Auto-save records before exit */
    saveAllHospitalRecords();

    /* Free all dynamically allocated memory */
    freeQueue();
    freeMemoryOnExit();

    printf("System closed cleanly. Goodbye, %s!\n", currentUser);
    return 0;
}