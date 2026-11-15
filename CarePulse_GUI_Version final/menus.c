#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <limits.h>
#include "structs.h"
#include "validation.h"
#include "security.h"
#include "member1.h"
#include "member2.h"
#include "member3.h"
#include "member4.h"
#include "menus.h"

/* Bonus Feature: Live Dashboard Banner at the top of the Main Menu */
static void printLiveDashboard(void)
{
    printf("\033[1;36m====================================================================\n");
    printf("               HOSPITAL MANAGEMENT & ER COMMAND SYSTEM\n");
    printf("====================================================================\033[0m\n");
    printf(" Patients: %-4d | ER Queue: %-4d | Ambulances Busy: %2d/%-2d\n",
        patientCount, queueSize, countActiveAmbulances(), VEHICLES * SLOTS);
    printf(" Medicines: %-3d | Revenue: PKR %-10.2f | Stamina: ",
        medicineCount, totalRevenue);
    printStaminaBar(doctorStamina);
    printf("\n\033[1;36m--------------------------------------------------------------------\033[0m\n");
}

void mainMenu(void)
{
    int choice;

    do {
        printLiveDashboard();
        printf("1. Admin Portal (Patients, Inventory, Reports & Charts)\n");
        printf("2. Pharmacy, Blood Bank & Patient Billing Desk\n");
        printf("3. Emergency Dispatcher & Ambulance Control\n");
        printf("4. Save & Exit System\n");
        printf("Enter choice (1-4): ");

        choice = getValidInt(1, 4);

        switch (choice) {
        case 1:
            adminMenu();
            break;
        case 2:
            pharmacyMenu();
            break;
        case 3:
            dispatcherMenu();
            break;
        case 4:
            printf("Saving data and exiting the system...\n");
            break;
        }
    } while (choice != 4);
}

void adminMenu(void)
{
    int choice, id;
    Patient* p;
    Medicine* m;

    if (!verifyAdminPin()) {
        pauseAndClear();
        return;
    }
    pauseAndClear();

    do {
        printf("\033[1;36m==================== ADMIN PORTAL ====================\033[0m\n");
        printf("1.  Add New Patient\n");
        printf("2.  View All Registered Patients\n");
        printf("3.  Update Patient Details\n");
        printf("4.  Delete Patient\n");
        printf("5.  Add New Medicine to Inventory\n");
        printf("6.  Update Medicine Stock\n");
        printf("7.  Delete Medicine\n");
        printf("8.  Change Admin PIN\n");
        printf("9.  Generate Operations Report & Visual Analytics Charts\n");
        printf("10. Save All Hospital Records to Disk\n");
        printf("11. Back to Main Menu\n");
        printf("Enter choice (1-11): ");

        choice = getValidInt(1, 11);

        switch (choice) {
        case 1:
            addPatient(patients, &patientCount);
            pauseAndClear();
            break;
        case 2:
            viewPatients(patients, patientCount);
            pauseAndClear();
            break;
        case 3:
            printf("Enter patient ID to update: ");
            id = getValidInt(1, INT_MAX);
            p = findPatientById(patients, patientCount, id);
            if (p != NULL) {
                updatePatient(p);
            }
            else {
                printf("\033[1;31mPatient not found.\033[0m\n");
            }
            pauseAndClear();
            break;
        case 4:
            printf("Enter patient ID to delete: ");
            id = getValidInt(1, INT_MAX);
            deletePatient(patients, &patientCount, id);
            pauseAndClear();
            break;
        case 5:
            addMedicineRecord();
            pauseAndClear();
            break;
        case 6:
            if (medicines == NULL || medicineCount == 0) {
                printf("No medicine records are available.\n");
            }
            else {
                printf("Enter medicine ID: ");
                id = getValidInt(1, INT_MAX);
                m = findMedicineById(medicines, medicineCount, id);
                if (m == NULL) {
                    printf("\033[1;31mMedicine not found.\033[0m\n");
                }
                else {
                    printf("Current stock for %s: %d\n", m->name, m->quantity);
                    printf("Enter stock change (+ to add, - to reduce): ");
                    updateMedicineStock(m, getValidInt(-1000000, 1000000));
                }
            }
            pauseAndClear();
            break;
        case 7:
            if (medicines == NULL || medicineCount == 0) {
                printf("No medicine records are available.\n");
            }
            else {
                printf("Enter medicine ID to delete: ");
                id = getValidInt(1, INT_MAX);
                deleteMedicine(medicines, &medicineCount, id);
            }
            pauseAndClear();
            break;
        case 8:
            changePin();
            pauseAndClear();
            break;
        case 9:
            generateHospitalReport();
            showVisualAnalyticsCharts();
            pauseAndClear();
            break;
        case 10:
            saveAllHospitalRecords();
            printf("\033[1;32mAll records saved manually.\033[0m\n");
            pauseAndClear();
            break;
        case 11:
            pauseAndClear();
            break;
        }
    } while (choice != 11);
}

void pharmacyMenu(void)
{
    int choice;

    pauseAndClear();
    do {
        printf("\033[1;36m=============== PHARMACY & BLOOD BANK DESK ===============\033[0m\n");
        printf("1. View Full Pharmacy Stock\n");
        printf("2. Update Medicine Stock\n");
        printf("3. Search Medicine by ID (Recursive Binary Search)\n");
        printf("4. Search Medicine by Name (Case-Insensitive)\n");
        printf("5. Sort Medicines (By Expiry Date or Triage Priority)\n");
        printf("6. Blood Bank Donor Compatibility Checker\n");
        printf("7. Calculate Patient Bill & Apply Discounts\n");
        printf("8. Back to Main Menu\n");
        printf("Enter choice (1-8): ");

        choice = getValidInt(1, 8);

        switch (choice) {
        case 1:
            displayMedicines();
            pauseAndClear();
            break;
        case 2:
            if (medicines == NULL || medicineCount == 0) {
                printf("No medicine records are available.\n");
            }
            else {
                int id, change;
                Medicine* m;
                printf("Enter medicine ID: ");
                id = getValidInt(1, INT_MAX);
                m = findMedicineById(medicines, medicineCount, id);
                if (m != NULL) {
                    printf("Enter stock change (+ to add, - to reduce): ");
                    change = getValidInt(-1000000, 1000000);
                    updateMedicineStock(m, change);
                }
                else {
                    printf("\033[1;31mMedicine not found.\033[0m\n");
                }
            }
            pauseAndClear();
            break;
        case 3:
            searchMedicineRecursiveMenu();
            pauseAndClear();
            break;
        case 4:
            searchMedicineByName();
            pauseAndClear();
            break;
        case 5: {
            int sortChoice;
            printf("\nSort By:\n1. Expiry Date (Selection Sort)\n2. Triage Priority (Bubble Sort)\nEnter choice: ");
            sortChoice = getValidInt(1, 2);
            if (sortChoice == 1) {
                sortMedicinesByExpiry();
                printf("\033[1;32mSorted by Expiry Date (Earliest first):\033[0m\n");
            }
            else {
                sortMedicinesByPriority();
                printf("\033[1;32mSorted by Triage Priority (Highest first):\033[0m\n");
            }
            displayMedicines();
            pauseAndClear();
            break;
        }
        case 6:
            checkBloodCompatibility();
            pauseAndClear();
            break;
        case 7:
            calculateBill();
            pauseAndClear();
            break;
        case 8:
            pauseAndClear();
            break;
        }
    } while (choice != 8);
}

void dispatcherMenu(void)
{
    int choice;

    pauseAndClear();
    do {
        printf("\033[1;36m============== EMERGENCY DISPATCHER & LOGISTICS ==============\033[0m\n");
        printf("1. View Emergency Priority Queue\n");
        printf("2. Quick-Add Walk-In Patient to Queue\n");
        printf("3. Treat Next Critical Patient (Uses Doctor Stamina)\n");
        printf("4. Doctor Stamina Status / Rest\n");
        printf("5. 2D Ambulance Grid (View / Book / Cancel)\n");
        printf("6. Search Registered Patient by Name\n");
        printf("7. Sort Patients by Triage Severity\n");
        printf("8. Simulate Mass-Casualty Event\n");
        printf("9. Back to Main Menu\n");
        printf("Enter choice (1-9): ");

        choice = getValidInt(1, 9);

        switch (choice) {
        case 1:
            displayQueue();
            pauseAndClear();
            break;
        case 2: {
            int id, sev, emerg;
            char name[MAX_NAME];
            printf("Enter Patient ID: ");
            id = getValidInt(1, INT_MAX);
            printf("Enter Patient Name: ");
            getValidString(name, sizeof(name));
            printf("Enter Severity (1-5): ");
            sev = getValidInt(1, 5);
            printf("Emergency Flag (1 = Yes, 0 = No): ");
            emerg = getValidInt(0, 1);
            if (enqueuePatient(id, name, sev, emerg)) {
                printf("\033[1;32mPatient inserted into priority queue.\033[0m\n");
            }
            pauseAndClear();
            break;
        }
        case 3:
            treatNextPatient(&doctorStamina);
            pauseAndClear();
            break;
        case 4:
            printf("Current Doctor Stamina: ");
            printStaminaBar(doctorStamina);
            printf("\nRest doctor back to full stamina? (1 = Yes, 0 = No): ");
            if (getValidInt(0, 1) == 1) {
                doctorStamina = STAMINA_MAX;
                printf("\033[1;32mDoctor rested! Stamina restored to %d/%d.\033[0m\n", doctorStamina, STAMINA_MAX);
            }
            pauseAndClear();
            break;
        case 5: {
            int subChoice, v, s;
            displayAmbulances();
            printf("\n1. Book Ambulance Slot\n2. Cancel Booking\n3. Back\nEnter choice: ");
            subChoice = getValidInt(1, 3);
            if (subChoice == 1) {
                printf("Enter Ambulance Number (1-%d): ", VEHICLES);
                v = getValidInt(1, VEHICLES);
                printf("Enter Time Slot (1-%d): ", SLOTS);
                s = getValidInt(1, SLOTS);
                bookAmbulance(v, s);
            }
            else if (subChoice == 2) {
                printf("Enter Ambulance Number (1-%d): ", VEHICLES);
                v = getValidInt(1, VEHICLES);
                printf("Enter Time Slot (1-%d): ", SLOTS);
                s = getValidInt(1, SLOTS);
                cancelAmbulance(v, s);
            }
            pauseAndClear();
            break;
        }
        case 6:
            searchPatientByName();
            pauseAndClear();
            break;
        case 7:
            sortPatientsByTriage();
            pauseAndClear();
            break;
        case 8: {
            int n;
            printf("Enter number of incoming mass-casualty patients (1-25): ");
            n = getValidInt(1, 25);
            massCasualtySimulation(n);
            pauseAndClear();
            break;
        }
        case 9:
            pauseAndClear();
            break;
        }
    } while (choice != 9);
}