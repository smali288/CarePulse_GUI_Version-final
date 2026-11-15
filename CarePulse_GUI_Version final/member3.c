#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "member3.h"
#include "member1.h"
#include "member4.h"
#include "validation.h"

int initializeInventory(int initialSize)
{
    medicineCapacity = initialSize;
    medicines = (Medicine*)malloc(medicineCapacity * sizeof(Medicine));
    if (medicines == NULL) {
        printf("\033[1;31mError: Initial pharmacy memory allocation failed!\033[0m\n");
        return 0;
    }
    medicineCount = 0;
    return 1;
}

void seedSampleInventory(void)
{
    Medicine samples[] = {
        { 101, "Paracetamol 500mg", 120, 50.0f,   20271215, 2 },
        { 102, "Epinephrine Amp",   8,   450.0f,  20261130, 5 },
        { 103, "Amoxicillin 250mg", 45,  180.0f,  20270510, 3 },
        { 104, "Insulin Regular",   0,   1200.0f, 20261201, 5 },
        { 105, "Sterile Bandage",   60,  85.0f,   20280820, 1 }
    };
    int count = 5;
    int i;

    if (medicines == NULL && !initializeInventory(INITIAL_MED_CAP)) {
        return;
    }

    for (i = 0; i < count; i++) {
        if (findMedicineById(medicines, medicineCount, samples[i].id) == NULL) {
            if (medicineCount >= medicineCapacity) {
                int newCap = medicineCapacity * 2;
                Medicine* temp = (Medicine*)realloc(medicines, newCap * sizeof(Medicine));
                if (temp == NULL) return;
                medicines = temp;
                medicineCapacity = newCap;
            }
            if (medicines != NULL && medicineCount < medicineCapacity) {
                medicines[medicineCount++] = samples[i];
            }
        }
    }
    printf("\033[1;32mSample pharmacy inventory loaded (5 essential items).\033[0m\n");
}

void addMedicineRecord(void)
{
    Medicine m;
    char input[MAX_NAME];

    if (medicines == NULL && !initializeInventory(INITIAL_MED_CAP)) {
        return;
    }

    if (medicineCount >= medicineCapacity) {
        int newCap = medicineCapacity * 2;
        Medicine* tempPointer = (Medicine*)realloc(medicines, newCap * sizeof(Medicine));
        if (tempPointer == NULL) {
            printf("\033[1;31mError: Memory reallocation failed!\033[0m\n");
            return;
        }
        medicines = tempPointer;
        medicineCapacity = newCap;
        printf("\033[1;36m[Inventory capacity automatically expanded to %d]\033[0m\n", medicineCapacity);
    }

    printf("\nEnter Medicine ID: ");
    m.id = getValidInt(1, INT_MAX);

    if (findMedicineById(medicines, medicineCount, m.id) != NULL) {
        printf("\033[1;31mError: A medicine with ID %d already exists.\033[0m\n", m.id);
        return;
    }

    printf("Enter Medicine Name: ");
    getValidString(input, sizeof(input));
    strncpy(m.name, input, MAX_NAME - 1);
    m.name[MAX_NAME - 1] = '\0';

    printf("Enter Stock Quantity: ");
    m.quantity = getValidInt(0, 1000000);

    printf("Enter Unit Price: ");
    m.price = getValidFloat(0.01f, 1000000.0f);

    while (1) {
        printf("Enter Expiry Date (YYYYMMDD, e.g. 20271231): ");
        m.expiry = getValidInt(20000101, 20991231);
        if (isValidExpiry(m.expiry)) {
            break;
        }
        printf("Invalid date format. Use YYYYMMDD with valid month/day.\n");
    }

    printf("Enter Triage Priority (1-5): ");
    m.triagePriority = getValidInt(1, 5);

    medicines[medicineCount++] = m;
    saveAllHospitalRecords();

    if (m.quantity == 0) {
        printf("\033[1;31mALERT: This medicine was added as OUT OF STOCK!\033[0m\n");
    }
    else if (m.quantity < 10) {
        printf("\033[1;33mWARNING: Low stock level detected (< 10 units)!\033[0m\n");
    }
    else {
        printf("\033[1;32mSuccess: Medicine added to dynamic inventory.\033[0m\n");
    }
}

void displayMedicines(void)
{
    int i;
    if (medicines == NULL || medicineCount == 0) {
        printf("\nPharmacy inventory is currently empty.\n");
        return;
    }

    printf("\n%-6s %-22s %-8s %-10s %-10s %-8s\n",
        "ID", "Medicine Name", "Stock", "Price", "Expiry", "Priority");
    printf("------------------------------------------------------------------------\n");

    for (i = 0; i < medicineCount; i++) {
        printf("%-6d %-22s %-8d %-10.2f %-10d %-8d",
            medicines[i].id,
            medicines[i].name,
            medicines[i].quantity,
            medicines[i].price,
            medicines[i].expiry,
            medicines[i].triagePriority);

        if (medicines[i].quantity == 0) {
            printf(" \033[1;31m[OUT OF STOCK]\033[0m");
        }
        else if (medicines[i].quantity < 10) {
            printf(" \033[1;33m[LOW STOCK]\033[0m");
        }
        printf("\n");
    }
    printf("(Items: %d / Allocated Capacity: %d)\n", medicineCount, medicineCapacity);
}

void sortMedicinesByExpiry(void)
{
    int i, j;
    if (medicines == NULL || medicineCount < 2) return;

    for (i = 0; i < medicineCount - 1; i++) {
        int minIndex = i;
        for (j = i + 1; j < medicineCount; j++) {
            if (medicines[j].expiry < medicines[minIndex].expiry) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            Medicine temp = medicines[i];
            medicines[i] = medicines[minIndex];
            medicines[minIndex] = temp;
        }
    }
}

void sortMedicinesByPriority(void)
{
    int i, j;
    if (medicines == NULL || medicineCount < 2) return;

    for (i = 0; i < medicineCount - 1; i++) {
        for (j = 0; j < medicineCount - i - 1; j++) {
            if (medicines[j].triagePriority < medicines[j + 1].triagePriority) {
                Medicine temp = medicines[j];
                medicines[j] = medicines[j + 1];
                medicines[j + 1] = temp;
            }
        }
    }
}

void sortMedicinesByID(void)
{
    int i, j;
    if (medicines == NULL || medicineCount < 2) return;

    for (i = 0; i < medicineCount - 1; i++) {
        for (j = 0; j < medicineCount - i - 1; j++) {
            if (medicines[j].id > medicines[j + 1].id) {
                Medicine temp = medicines[j];
                medicines[j] = medicines[j + 1];
                medicines[j + 1] = temp;
            }
        }
    }
}

void sortPatientsByTriage(void)
{
    int i, j;
    if (patientCount == 0) {
        printf("\nNo patients registered to sort.\n");
        return;
    }

    for (i = 0; i < patientCount - 1; i++) {
        for (j = 0; j < patientCount - i - 1; j++) {
            if (patients[j].severity < patients[j + 1].severity) {
                Patient temp = patients[j];
                patients[j] = patients[j + 1];
                patients[j + 1] = temp;
            }
        }
    }
    printf("\033[1;32mPatients sorted by triage severity (Highest to Lowest):\033[0m\n");
    viewPatients(patients, patientCount);
}

int binarySearchByID(int low, int high, int targetID)
{
    if (medicines == NULL) return -1;

    if (high >= low) {
        int mid = low + (high - low) / 2;
        if (medicines[mid].id == targetID) {
            return mid;
        }
        if (medicines[mid].id > targetID) {
            return binarySearchByID(low, mid - 1, targetID);
        }
        return binarySearchByID(mid + 1, high, targetID);
    }
    return -1;
}

void searchMedicineRecursiveMenu(void)
{
    int searchID, result;
    if (medicines == NULL || medicineCount == 0) {
        printf("\nInventory is empty.\n");
        return;
    }

    printf("Enter Medicine ID for recursive binary search: ");
    searchID = getValidInt(1, INT_MAX);

    sortMedicinesByID();
    result = binarySearchByID(0, medicineCount - 1, searchID);

    if (result != -1) {
        printf("\033[1;32mMedicine Found!\033[0m ID: %d | Name: %s | Stock: %d | Price: %.2f | Expiry: %d\n",
            medicines[result].id, medicines[result].name,
            medicines[result].quantity, medicines[result].price, medicines[result].expiry);
    }
    else {
        printf("\033[1;31mMedicine with ID %d not found.\033[0m\n", searchID);
    }
}

void searchMedicineByName(void)
{
    char query[MAX_NAME];
    int foundCount = 0;
    int i, k;

    if (medicines == NULL || medicineCount == 0) {
        printf("\nInventory is empty.\n");
        return;
    }

    printf("Enter medicine name or keyword: ");
    getValidString(query, sizeof(query));

    printf("\n--- MEDICINE SEARCH RESULTS ---\n");
    for (i = 0; i < medicineCount; i++) {
        char tempName[MAX_NAME], tempQuery[MAX_NAME];
        for (k = 0; medicines[i].name[k]; k++) {
            tempName[k] = (char)tolower((unsigned char)medicines[i].name[k]);
        }
        tempName[k] = '\0';

        for (k = 0; query[k]; k++) {
            tempQuery[k] = (char)tolower((unsigned char)query[k]);
        }
        tempQuery[k] = '\0';

        if (strstr(tempName, tempQuery) != NULL) {
            printf("ID: %-5d | Name: %-20s | Stock: %-4d | Price: %.2f\n",
                medicines[i].id, medicines[i].name, medicines[i].quantity, medicines[i].price);
            foundCount++;
        }
    }

    if (foundCount == 0) {
        printf("No matching medicines found for '%s'.\n", query);
    }
}

void searchPatientByName(void)
{
    char query[MAX_NAME];
    int foundCount = 0;
    int i, k;

    if (patientCount == 0) {
        printf("\nNo patients registered.\n");
        return;
    }

    printf("Enter patient name or keyword: ");
    getValidString(query, sizeof(query));

    printf("\n--- PATIENT SEARCH RESULTS ---\n");
    for (i = 0; i < patientCount; i++) {
        char tempName[MAX_NAME], tempQuery[MAX_NAME];
        for (k = 0; patients[i].name[k]; k++) {
            tempName[k] = (char)tolower((unsigned char)patients[i].name[k]);
        }
        tempName[k] = '\0';

        for (k = 0; query[k]; k++) {
            tempQuery[k] = (char)tolower((unsigned char)query[k]);
        }
        tempQuery[k] = '\0';

        if (strstr(tempName, tempQuery) != NULL) {
            printf("ID: %-5d | Name: %-20s | Blood: %-4s | Severity: %d\n",
                patients[i].id, patients[i].name, patients[i].bloodGroup, patients[i].severity);
            foundCount++;
        }
    }
    if (foundCount == 0) {
        printf("No matching patients found for '%s'.\n", query);
    }
}
void checkBloodCompatibility(void)
{
    Donor donorList[MAX_DONORS] = {
    { 201, "Ayesha Siddiqui", "A-"  },
    { 202, "Bilal Ahmed",     "A+"  },
    { 203, "Fatima Tariq",    "AB-" },
    { 204, "Hassan Raza",     "AB+" },
    { 205, "Waqas Mahmood",   "B-"  },
    { 206, "Zainab Bibi",     "B+"  },
    { 207, "Ali Khan",        "O-"  },
    { 208, "Usman Ghani",     "O+"  }
    };
    char patientGroup[8];
    int matchFound = 0;
    int i;
    while (1) {
        printf("Enter recipient blood group (A+, A-, B+, B-, AB+, AB-, O+, O-): ");
        getValidString(patientGroup, sizeof(patientGroup));
        for (i = 0; patientGroup[i]; i++) {
            patientGroup[i] = (char)toupper((unsigned char)patientGroup[i]);
        }
        if (isValidBloodGroup(patientGroup)) {
            break;
        }
        printf("Invalid blood group.\n");
    }
    printf("\n--- COMPATIBLE DONORS FOR [%s] ---\n", patientGroup);
    for (i = 0; i < MAX_DONORS; i++) {
        int isCompatible = 0;
        const char* d = donorList[i].bloodGroup;
        if (strcmp(d, "O-") == 0) {
            isCompatible = 1;
        }
        else if (strcmp(patientGroup, "AB+") == 0) {
            isCompatible = 1;
        }
        else if (strcmp(d, patientGroup) == 0) {
            isCompatible = 1;
        }
        else if (strcmp(d, "O+") == 0 &&
            (strcmp(patientGroup, "A+") == 0 || strcmp(patientGroup, "B+") == 0)) {
            isCompatible = 1;
        }
        else if (strcmp(d, "A-") == 0 &&
            (strcmp(patientGroup, "A+") == 0 || strcmp(patientGroup, "AB-") == 0)) {
            isCompatible = 1;
        }
        else if (strcmp(d, "B-") == 0 &&
            (strcmp(patientGroup, "B+") == 0 || strcmp(patientGroup, "AB-") == 0)) {
            isCompatible = 1;
        }
        if (isCompatible) {
            printf("Donor ID: %-4d | Name: %-18s | Blood Group: \033[1;32m%-4s\033[0m\n",
                donorList[i].donorID, donorList[i].donorName, donorList[i].bloodGroup);
            matchFound++;
        }
    }
    if (matchFound == 0) {
        printf("No compatible donors found.\n");
    }
}
void freeMemoryOnExit(void)
{
    if (medicines != NULL) {
        free(medicines);
        medicines = NULL;
        medicineCapacity = 0;
        medicineCount = 0;
    }
}