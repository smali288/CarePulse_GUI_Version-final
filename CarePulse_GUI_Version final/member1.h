#define _CRT_SECURE_NO_WARNINGS
#ifndef MEMBER1_H
#define MEMBER1_H

#include "structs.h"

void addPatient(Patient arr[], int* count);
Patient* findPatientById(Patient arr[], int count, int id);
void updatePatient(Patient* p);
int deletePatient(Patient arr[], int* count, int id);
void viewPatients(Patient arr[], int count);

Medicine* findMedicineById(Medicine arr[], int count, int id);
void updateMedicineStock(Medicine* m, int change);
int deleteMedicine(Medicine arr[], int* count, int id);

#endif
