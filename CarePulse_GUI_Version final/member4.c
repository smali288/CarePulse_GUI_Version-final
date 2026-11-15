#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "member4.h"
#include "member1.h"
#include "member2.h"
#include "member3.h"
#include "validation.h"

float calculateRoomCost(int roomType, int days)
{
    float pricePerDay = 0.0f;
    switch (roomType) {
    case 1:
        pricePerDay = 2000.0f; /* General Ward */
        break;
    case 2:
        pricePerDay = 5000.0f; /* Private Room */
        break;
    case 3:
        pricePerDay = 10000.0f; /* ICU */
        break;
    default:
        pricePerDay = 0.0f;
    }
    return pricePerDay * days;
}

float calculateDiscount(float subtotal, int age, int income, float* discountApplied)
{
    float totalDiscount = 0.0f;
    float seniorDiscount = 0.0f;
    float charityDiscount = 0.0f;
    float remainingAmount = subtotal;

    if (age >= 60) {
        seniorDiscount = subtotal * 0.20f;
        totalDiscount += seniorDiscount;
        printf(" -> Senior Citizen Discount (20%%): -%.2f\n", seniorDiscount);
    }

    remainingAmount = subtotal - seniorDiscount;

    if (income <= 50000) {
        if (fundBalance <= 0.0f) {
            printf(" -> Low-income charity fund is currently depleted.\n");
        }
        else {
            charityDiscount = remainingAmount * 0.50f;
            if (charityDiscount <= fundBalance) {
                fundBalance -= charityDiscount;
            }
            else {
                charityDiscount = fundBalance;
                fundBalance = 0.0f;
            }
            totalDiscount += charityDiscount;
            remainingAmount -= charityDiscount;
            printf(" -> Low-Income Charity Fund Aid (50%%): -%.2f\n", charityDiscount);
        }
    }

    if (discountApplied != NULL) {
        *discountApplied = totalDiscount;
    }

    return remainingAmount;
}

/* Bonus Feature: Exports an itemized receipt to a .txt file */
void exportBillReceipt(const Bill* b)
{
    char filename[64];
    FILE* fp;
    if (b == NULL) return;

    sprintf(filename, "receipt_bill_%d.txt", b->billId);
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("\033[1;33mWarning: Could not export receipt file.\033[0m\n");
        return;
    }

    fprintf(fp, "==========================================\n");
    fprintf(fp, "       HOSPITAL MANAGEMENT SYSTEM         \n");
    fprintf(fp, "            OFFICIAL RECEIPT              \n");
    fprintf(fp, "==========================================\n");
    fprintf(fp, "Bill ID        : %d\n", b->billId);
    fprintf(fp, "Patient ID     : %d\n", b->patientId);
    fprintf(fp, "Consultation   : PKR %.2f\n", b->consultFee);
    fprintf(fp, "Room Charges   : PKR %.2f (%d days)\n", b->roomCost, b->days);
    fprintf(fp, "Pharmacy Cost  : PKR %.2f\n", b->pharmacyCost);
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "Subtotal       : PKR %.2f\n", b->subtotal);
    fprintf(fp, "Discounts      : -PKR %.2f\n", b->discount);
    fprintf(fp, "Net Payable    : PKR %.2f\n", b->total);
    fprintf(fp, "Payment Status : %s\n", b->isPaid ? "PAID" : "UNPAID / PENDING");
    fprintf(fp, "==========================================\n");
    fclose(fp);

    printf("\033[1;36mReceipt saved to disk: %s\033[0m\n", filename);
}

void calculateBill(void)
{
    Bill newBill;
    Patient* existing;

    printf("\n--- PATIENT BILLING & DISCOUNT CALCULATOR ---\n");
    printf("Enter Bill ID: ");
    newBill.billId = getValidInt(1, INT_MAX);

    printf("Enter Patient ID: ");
    newBill.patientId = getValidInt(1, INT_MAX);

    /* Auto-fetch age if patient is already registered */
    existing = findPatientById(patients, patientCount, newBill.patientId);
    if (existing != NULL) {
        newBill.age = existing->age;
        printf("Patient Found: %s (Auto-filled Age: %d)\n", existing->name, newBill.age);
    }
    else {
        printf("Enter Patient Age (0-120): ");
        newBill.age = getValidInt(0, 120);
    }

    printf("Enter Monthly Household Income (PKR): ");
    newBill.income = getValidInt(0, 100000000);

    printf("Enter Consultation Fee: ");
    newBill.consultFee = getValidFloat(0.0f, 1000000.0f);

    printf("Select Room Type:\n");
    printf("  0. Outpatient (None)\n");
    printf("  1. General Ward (2000/day)\n");
    printf("  2. Private Room (5000/day)\n");
    printf("  3. Intensive Care / ICU (10000/day)\n");
    printf("Enter Room Type (0-3): ");
    newBill.roomType = getValidInt(0, 3);

    if (newBill.roomType > 0) {
        printf("Enter Number of Days Stayed (1-365): ");
        newBill.days = getValidInt(1, 365);
        newBill.roomCost = calculateRoomCost(newBill.roomType, newBill.days);
    }
    else {
        newBill.days = 0;
        newBill.roomCost = 0.0f;
    }

    printf("Enter Pharmacy Cost: ");
    newBill.pharmacyCost = getValidFloat(0.0f, 1000000.0f);

    newBill.subtotal = newBill.consultFee + newBill.roomCost + newBill.pharmacyCost;
    newBill.total = calculateDiscount(newBill.subtotal, newBill.age, newBill.income, &newBill.discount);

    printf("\n------------------------------------------\n");
    printf("Subtotal         : PKR %.2f\n", newBill.subtotal);
    printf("Total Discount   : -PKR %.2f\n", newBill.discount);
    printf("Final Amount Due : \033[1;32mPKR %.2f\033[0m\n", newBill.total);
    printf("------------------------------------------\n");

    printf("Payment Status (1 = Paid Now, 0 = Unpaid/Pending): ");
    newBill.isPaid = getValidInt(0, 1);

    if (newBill.isPaid == 1) {
        paidBills++;
        totalRevenue += newBill.total;
        printf("\033[1;32mPayment recorded. Revenue updated.\033[0m\n");
    }
    else {
        unpaidBills++;
        printf("\033[1;33mBill marked as UNPAID.\033[0m\n");
    }

    exportBillReceipt(&newBill);
    saveAllHospitalRecords();
}

int countRestockItems(void)
{
    int i, restock = 0;
    for (i = 0; i < medicineCount; i++) {
        if (medicines[i].quantity < 10) {
            restock++;
        }
    }
    return restock;
}

void generateHospitalReport(void)
{
    int i;
    int activeAmb = countActiveAmbulances();
    int restockCount = countRestockItems();

    printf("\n========================================================\n");
    printf("       DAILY HOSPITAL OPERATIONS & FINANCIAL REPORT\n");
    printf("========================================================\n");
    printf("Total Patient Arrivals (All-Time): %d\n", arrivalCounter);
    printf("Currently Registered Patients    : %d\n", patientCount);
    printf("Patients Waiting in ER Queue     : %d\n", queueSize);
    printf("Active Ambulance Dispatches      : %d / %d slots\n", activeAmb, VEHICLES * SLOTS);
    printf("Total Collected Revenue          : PKR %.2f\n", totalRevenue);
    printf("Paid Bills / Unpaid Bills        : %d Paid | %d Unpaid\n", paidBills, unpaidBills);
    printf("Remaining Charity Fund Balance   : PKR %.2f\n", fundBalance);
    printf("Medicines Needing Restock (<10)  : %d items\n", restockCount);

    if (restockCount > 0) {
        printf("\n--- RESTOCK ALERT LIST ---\n");
        for (i = 0; i < medicineCount; i++) {
            if (medicines[i].quantity < 10) {
                printf(" -> [ID %d] %s (Current Stock: %d)\n",
                    medicines[i].id, medicines[i].name, medicines[i].quantity);
            }
        }
    }
    printf("========================================================\n");
}

/* Bonus Feature: Terminal Bar Graphs & Proportional Pie Charts */
void showVisualAnalyticsCharts(void)
{
    int sevCounts[6] = { 0 };
    int emergencyCount = 0, routineCount = 0;
    int i, b, barLen;

    for (i = 0; i < patientCount; i++) {
        if (patients[i].severity >= 1 && patients[i].severity <= 5) {
            sevCounts[patients[i].severity]++;
        }
        if (patients[i].emergencyFlag) emergencyCount++;
        else routineCount++;
    }

    printf("\n\033[1;36m==============================================================\n");
    printf("        HOSPITAL VISUAL ANALYTICS & FORECAST CHARTS\n");
    printf("==============================================================\033[0m\n");

    /* Chart 1: Bar Graph of Patient Triage Severity */
    printf("\n[1] PATIENT TRIAGE SEVERITY BAR GRAPH (Total: %d)\n", patientCount);
    for (i = 1; i <= 5; i++) {
        barLen = (patientCount > 0) ? (sevCounts[i] * 30) / patientCount : 0;
        printf("Level %d | ", i);
        if (i >= 4) printf("\033[1;31m");
        else if (i == 3) printf("\033[1;33m");
        else printf("\033[1;32m");

        for (b = 0; b < barLen; b++) printf("#");
        for (b = barLen; b < 30; b++) printf(".");
        printf("\033[0m (%d patients)\n", sevCounts[i]);
    }

    /* Chart 2: Proportional Pie-Segment Bar (Emergency vs Routine) */
    printf("\n[2] CASE MIX PIE BREAKDOWN (Emergency vs. Routine)\n");
    if (patientCount > 0) {
        int emergPct = (emergencyCount * 100) / patientCount;
        int routPct = 100 - emergPct;
        int emergBlocks = (emergPct * 30) / 100;

        printf("    [\033[1;31m");
        for (b = 0; b < emergBlocks; b++) printf("E");
        printf("\033[1;32m");
        for (b = emergBlocks; b < 30; b++) printf("R");
        printf("\033[0m]\n");
        printf("    \033[1;31m[E] Emergency Cases: %d%%\033[0m  |  \033[1;32m[R] Routine Cases: %d%%\033[0m\n",
            emergPct, routPct);
    }
    else {
        printf("    No patient data yet to plot.\n");
    }

    /* Chart 3: Past vs. Future Patient Load Forecast (+25% Growth Model) */
    printf("\n[3] PAST vs. PROJECTED FUTURE PATIENT ARRIVALS\n");
    {
        int past = arrivalCounter;
        int current = patientCount;
        int futureForecast = (int)(current * 1.25f) + 2;
        int maxVal = (past > futureForecast) ? past : futureForecast;
        if (maxVal == 0) maxVal = 1;

        printf("Past Recorded Arrivals | ");
        for (b = 0; b < (past * 25) / maxVal; b++) printf("=");
        printf(" (%d)\n", past);
        printf("Current Active Ward    | ");
        for (b = 0; b < (current * 25) / maxVal; b++) printf("=");
        printf(" (%d)\n", current);
        printf("Projected Next-Day     | \033[1;36m");
        for (b = 0; b < (futureForecast * 25) / maxVal; b++) printf("=");
        printf("\033[0m (%d est.)\n", futureForecast);
    }
    printf("==============================================================\n");
}
void saveAllHospitalRecords(void)
{
    int i, v, s;
    FILE* file = fopen("hospital_master_records.txt", "w");
    if (file == NULL) {
        printf("\033[1;31mError: File write failure! Could not save records to disk.\033[0m\n");
        return;
    }
    /* Section 1: Financial & System Counters */
    fprintf(file, "%.2f %d %d %.2f %d %d %d\n",
        totalRevenue, paidBills, unpaidBills, fundBalance,
        arrivalCounter, patientCount, medicineCount);
    /* Section 2: Patient Records */
    for (i = 0; i < patientCount; i++) {
        fprintf(file, "%d|%s|%d|%c|%s|%s|%d|%d|%d\n",
            patients[i].id, patients[i].name, patients[i].age,
            patients[i].gender, patients[i].phone, patients[i].bloodGroup,
            patients[i].severity, patients[i].emergencyFlag, patients[i].arrivalOrder);
    }
    /* Section 3: Medicine Inventory */
    for (i = 0; i < medicineCount; i++) {
        fprintf(file, "%d|%s|%d|%.2f|%d|%d\n",
            medicines[i].id, medicines[i].name, medicines[i].quantity,
            medicines[i].price, medicines[i].expiry, medicines[i].triagePriority);
    }
    /* Section 4: 2D Ambulance Grid */
    for (v = 0; v < VEHICLES; v++) {
        for (s = 0; s < SLOTS; s++) {
            fprintf(file, "%d ", ambulance[v][s]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
void loadAllHospitalRecords(void)
{
    int i, v, s, savedPatients = 0, savedMeds = 0;
    FILE* file = fopen("hospital_master_records.txt", "r");
    if (file == NULL) {
        printf("Notice: No previous save file found. Loading starter pharmacy stock...\n");
        seedSampleInventory();
        saveAllHospitalRecords();
        return;
    }
    if (fscanf(file, "%f %d %d %f %d %d %d\n",
        &totalRevenue, &paidBills, &unpaidBills, &fundBalance,
        &arrivalCounter, &savedPatients, &savedMeds) != 7) {
        printf("\033[1;33mWarning: Save file format unreadable. Starting fresh.\033[0m\n");
        fclose(file);
        seedSampleInventory();
        return;
    }
    /* Load Patients */
    patientCount = 0;
    for (i = 0; i < savedPatients && i < MAX_PATIENTS; i++) {
        Patient p;
        if (fscanf(file, "%d|%30[^|]|%d|%c|%11[^|]|%3[^|]|%d|%d|%d\n",
            &p.id, p.name, &p.age, &p.gender, p.phone,
            p.bloodGroup, &p.severity, &p.emergencyFlag, &p.arrivalOrder) == 9) {
            patients[patientCount++] = p;
            if (p.emergencyFlag) {
                enqueueExistingPatient(&p);
            }
        }
    }
    /* Load Medicines /
    medicineCount = 0;
    for (i = 0; i < savedMeds; i++) {
    Medicine m;
    if (fscanf(file, "%d|%30[^|]|%d|%f|%d|%d\n",
    &m.id, m.name, &m.quantity, &m.price, &m.expiry, &m.triagePriority) == 6) {
    if (medicineCount >= medicineCapacity) {
    int newCap = medicineCapacity * 2;
    Medicine temp = (Medicine*)realloc(medicines, newCap * sizeof(Medicine));
    if (temp != NULL) {
    medicines = temp;
    medicineCapacity = newCap;
    }
    }
    if (medicineCount < medicineCapacity) {
    medicines[medicineCount++] = m;
    }
    }
    }
    /* Load 2D Ambulance Grid */
    for (v = 0; v < VEHICLES; v++) {
        for (s = 0; s < SLOTS; s++) {
            fscanf(file, "%d", &ambulance[v][s]);
        }
    }
    fclose(file);
    printf("\033[1;32mSuccess: Loaded %d patients, %d medicines, and financial records.\033[0m\n",
        patientCount, medicineCount);
}