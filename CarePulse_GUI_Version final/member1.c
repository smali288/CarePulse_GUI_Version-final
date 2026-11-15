#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "member1.h"
#include "member2.h"
#include "member4.h"
#include "validation.h"

void addPatient(Patient arr[], int* count)
{
    Patient p;
    char input[100];
    int i;

    if (arr == NULL || count == NULL || *count >= MAX_PATIENTS) {
        printf("\033[1;31mPatient list is full or unavailable.\033[0m\n");
        return;
    }

    printf("\nEnter patient ID: ");
    p.id = getValidInt(1, INT_MAX);

    if (findPatientById(arr, *count, p.id) != NULL) {
        printf("\033[1;31mA patient with this ID already exists.\033[0m\n");
        return;
    }

    while (1) {
        printf("Enter patient name: ");
        getValidString(input, sizeof(input));
        if (isValidName(input)) {
            strcpy(p.name, input);
            break;
        }
        printf("Name must contain only letters and spaces.\n");
    }

    printf("Enter age (0-120): ");
    p.age = getValidInt(0, 120);

    while (1) {
        printf("Enter gender (M/F/O): ");
        getValidString(input, sizeof(input));
        if (input[0] != '\0' && input[1] == '\0' && isValidGender(input[0])) {
            p.gender = (char)toupper((unsigned char)input[0]);
            break;
        }
        printf("Please enter M, F, or O.\n");
    }

    while (1) {
        printf("Enter 11-digit phone number: ");
        getValidString(input, sizeof(input));
        if (isValidPhone(input)) {
            strcpy(p.phone, input);
            break;
        }
        printf("Phone number must contain exactly 11 digits.\n");
    }

    while (1) {
        printf("Enter blood group (A+, A-, B+, B-, AB+, AB-, O+, O-): ");
        getValidString(input, sizeof(input));
        for (i = 0; input[i]; i++) {
            input[i] = (char)toupper((unsigned char)input[i]);
        }
        if (isValidBloodGroup(input)) {
            strcpy(p.bloodGroup, input);
            break;
        }
        printf("Invalid blood group.\n");
    }

    printf("Enter severity (1-5): ");
    p.severity = getValidInt(1, 5);
    p.emergencyFlag = (p.severity >= 4) ? 1 : 0;
    p.arrivalOrder = ++arrivalCounter;

    arr[*count] = p;
    (*count)++;

    printf("\033[1;32mPatient added successfully.\033[0m\n");

    if (p.emergencyFlag) {
        printf("\033[1;31m[EMERGENCY ALERT] High severity! Auto-routing to Emergency Queue...\033[0m\n");
        enqueueExistingPatient(&p);
    }

    saveAllHospitalRecords();
}

Patient* findPatientById(Patient arr[], int count, int id)
{
    int i;
    for (i = 0; i < count; i++) {
        if (arr[i].id == id) {
            return &arr[i];
        }
    }
    return NULL;
}

void updatePatient(Patient* p)
{
    int choice;
    int oldSeverity;
    char input[100];

    if (p == NULL) {
        printf("Patient not found.\n");
        return;
    }

    do {
        printf("\n--- Update Patient (%s | ID: %d) ---\n", p->name, p->id);
        printf("1. Update name\n");
        printf("2. Update age\n");
        printf("3. Update phone\n");
        printf("4. Update severity\n");
        printf("5. Back\n");
        printf("Enter choice: ");

        choice = getValidInt(1, 5);

        switch (choice) {
        case 1:
            while (1) {
                printf("Enter new name: ");
                getValidString(input, sizeof(input));
                if (isValidName(input)) {
                    strcpy(p->name, input);
                    saveAllHospitalRecords();
                    printf("\033[1;32mName updated.\033[0m\n");
                    break;
                }
                printf("Invalid name. Use letters and spaces only.\n");
            }
            break;

        case 2:
            printf("Enter new age (0-120): ");
            p->age = getValidInt(0, 120);
            saveAllHospitalRecords();
            printf("\033[1;32mAge updated.\033[0m\n");
            break;

        case 3:
            while (1) {
                printf("Enter new 11-digit phone number: ");
                getValidString(input, sizeof(input));
                if (isValidPhone(input)) {
                    strcpy(p->phone, input);
                    saveAllHospitalRecords();
                    printf("\033[1;32mPhone updated.\033[0m\n");
                    break;
                }
                printf("Invalid phone number.\n");
            }
            break;

        case 4:
            oldSeverity = p->severity;
            printf("Enter new severity (1-5): ");
            p->severity = getValidInt(1, 5);
            p->emergencyFlag = (p->severity >= 4) ? 1 : 0;
            printf("\033[1;32mSeverity updated.\033[0m\n");

            if (oldSeverity != p->severity) {
                removeFromQueueById(p->id);
                if (p->emergencyFlag) {
                    printf("\033[1;33mPatient escalated to emergency! Re-inserted into priority queue.\033[0m\n");
                    enqueueExistingPatient(p);
                }
            }
            saveAllHospitalRecords();
            break;

        case 5:
            break;
        }
    } while (choice != 5);
}

int deletePatient(Patient arr[], int* count, int id)
{
    int i, j;
    char answer[10];

    if (arr == NULL || count == NULL) {
        return 0;
    }

    for (i = 0; i < *count; i++) {
        if (arr[i].id == id) {
            printf("Delete patient %s? (Y/N): ", arr[i].name);
            getValidString(answer, sizeof(answer));

            if (answer[0] == '\0' || toupper((unsigned char)answer[0]) != 'Y' || answer[1] != '\0') {
                printf("Deletion cancelled.\n");
                return 0;
            }

            for (j = i; j < *count - 1; j++) {
                arr[j] = arr[j + 1];
            }
            (*count)--;

            removeFromQueueById(id);
            saveAllHospitalRecords();

            printf("\033[1;32mPatient deleted successfully.\033[0m\n");
            return 1;
        }
    }

    printf("\033[1;31mPatient not found.\033[0m\n");
    return 0;
}

void viewPatients(Patient arr[], int count)
{
    int i;
    if (count == 0) {
        printf("\nNo patients registered.\n");
        return;
    }

    printf("\n%-6s %-20s %-4s %-6s %-12s %-6s %-4s %-9s\n",
        "ID", "Name", "Age", "Gender", "Phone", "Blood", "Sev", "Status");
    printf("----------------------------------------------------------------------------\n");

    for (i = 0; i < count; i++) {
        printf("%-6d %-20s %-4d %-6c %-12s %-6s %-4d %s\n",
            arr[i].id,
            arr[i].name,
            arr[i].age,
            arr[i].gender,
            arr[i].phone,
            arr[i].bloodGroup,
            arr[i].severity,
            arr[i].emergencyFlag ? "\033[1;31mEMERGENCY\033[0m" : "Normal");
    }
}

Medicine* findMedicineById(Medicine arr[], int count, int id)
{
    int i;
    if (arr == NULL) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        if (arr[i].id == id) {
            return &arr[i];
        }
    }
    return NULL;
}

void updateMedicineStock(Medicine* m, int change)
{
    if (m == NULL) {
        printf("Medicine not found.\n");
        return;
    }

    if (m->quantity + change < 0) {
        printf("\033[1;31mStock cannot become negative.\033[0m\n");
        return;
    }

    m->quantity += change;
    saveAllHospitalRecords();
    printf("\033[1;32mStock updated. New quantity: %d\033[0m\n", m->quantity);

    if (m->quantity == 0) {
        printf("\033[1;31mALERT: This medicine is OUT OF STOCK!\033[0m\n");
    }
    else if (m->quantity < 10) {
        printf("\033[1;33mWARNING: Low stock level (< 10 units).\033[0m\n");
    }
}

int deleteMedicine(Medicine arr[], int* count, int id)
{
    int i, j;
    char answer[10];

    if (arr == NULL || count == NULL) {
        return 0;
    }

    for (i = 0; i < *count; i++) {
        if (arr[i].id == id) {
            printf("Delete medicine %s? (Y/N): ", arr[i].name);
            getValidString(answer, sizeof(answer));

            if (answer[0] == '\0' || toupper((unsigned char)answer[0]) != 'Y' || answer[1] != '\0') {
                printf("Deletion cancelled.\n");
                return 0;
            }

            for (j = i; j < *count - 1; j++) {
                arr[j] = arr[j + 1];
            }
            (*count)--;

            if (medicineCapacity > INITIAL_MED_CAP && *count <= medicineCapacity / 4) {
                int newCap = medicineCapacity / 2;
                Medicine* shrunk = (Medicine*)realloc(medicines, newCap * sizeof(Medicine));
                if (shrunk != NULL) {
                    medicines = shrunk;
                    medicineCapacity = newCap;
                    printf("[Inventory capacity auto-shrunk to %d]\n", medicineCapacity);
                }
            }

            saveAllHospitalRecords();
            printf("\033[1;32mMedicine deleted successfully.\033[0m\n");
            return 1;
        }
    }

    printf("\033[1;31mMedicine not found.\033[0m\n");
    return 0;
}
