#define _CRT_SECURE_NO_WARNINGS
#ifndef MEMBER4_H
#define MEMBER4_H

#include "structs.h"

/* Billing & Discounts */
float calculateRoomCost(int roomType, int days);
float calculateDiscount(float subtotal, int age, int income, float* discountApplied);
void calculateBill(void);
void exportBillReceipt(const Bill* b);

/* Reporting & Visual Charts (Bonus Feature) */
int countRestockItems(void);
void generateHospitalReport(void);
void showVisualAnalyticsCharts(void);

/* Persistent File Save & Load */
void saveAllHospitalRecords(void);
void loadAllHospitalRecords(void);

#endif
