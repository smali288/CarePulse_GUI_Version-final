#define _CRT_SECURE_NO_WARNINGS
#ifndef MEMBER3_H
#define MEMBER3_H

#include "structs.h"

/* Dynamic Pharmacy Inventory */
int initializeInventory(int initialSize);
void seedSampleInventory(void);
void addMedicineRecord(void);
void displayMedicines(void);
void freeMemoryOnExit(void);

/* Sorting Algorithms */
void sortMedicinesByExpiry(void);
void sortMedicinesByPriority(void);
void sortMedicinesByID(void);
void sortPatientsByTriage(void);

/* Searching Algorithms (Includes Required Recursion) */
int binarySearchByID(int low, int high, int targetID);
void searchMedicineRecursiveMenu(void);
void searchMedicineByName(void);
void searchPatientByName(void);

/* Blood Bank System */
void checkBloodCompatibility(void);

#endif
