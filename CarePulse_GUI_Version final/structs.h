
#define _CRT_SECURE_NO_WARNINGS
#ifndef STRUCTS_H
#define STRUCTS_H

/* System-wide constants */
#define MAX_PATIENTS 100
#define MAX_NAME 31
#define MAX_ATTEMPTS 3
#define PIN_LEN 7
#define DEFAULT_PIN "1234"

/* Member 2 constants */
#define VEHICLES 4
#define SLOTS 6
#define INITIAL_QUEUE_CAP 4
#define STAMINA_MAX 10

/* Member 3 constants */
#define INITIAL_MED_CAP 4
#define MAX_DONORS 8

/* Unified Patient Structure (Members 1 & 2) */
typedef struct {
    int id;                     /* Unique patient ID */
    char name[MAX_NAME];        /* Patient's name */
    int age;                    /* Patient's age */
    char gender;                /* M, F, or O */
    char phone[12];             /* 11 digits plus '\0' */
    char bloodGroup[4];         /* Blood group (e.g., AB+) */
    int severity;               /* Emergency severity (1 to 5) */
    int emergencyFlag;          /* 1 = emergency, 0 = normal */
    int arrivalOrder;           /* Arrival sequence number */
} Patient;

/* Unified Medicine Structure (Members 1 & 3) */
typedef struct {
    int id;                     /* Unique medicine ID */
    char name[MAX_NAME];        /* Medicine name */
    int quantity;               /* Available stock quantity */
    float price;                /* Unit price */
    int expiry;                 /* Expiry date as YYYYMMDD */
    int triagePriority;         /* Medical priority (1 to 5) */
} Medicine;

/* Blood Bank Donor Structure (Member 3) */
typedef struct {
    int donorID;
    char donorName[MAX_NAME];
    char bloodGroup[4];
} Donor;

/* Billing Record Structure (Member 4) */
typedef struct {
    int billId;
    int patientId;
    int age;
    int income;
    int roomType;
    int days;
    float consultFee;
    float roomCost;
    float pharmacyCost;
    float subtotal;
    float discount;
    float total;
    int isPaid;
} Bill;

/* Shared Global Variables (Defined once in main.c) */
extern Patient patients[MAX_PATIENTS];
extern int patientCount;
extern int arrivalCounter;

/* Member 2 Globals */
extern Patient* queue;
extern int queueSize;
extern int queueCapacity;
extern int ambulance[VEHICLES][SLOTS];
extern int doctorStamina;

/* Member 3 Globals */
extern Medicine* medicines;
extern int medicineCount;
extern int medicineCapacity;

/* Member 4 Globals */
extern float totalRevenue;
extern int paidBills;
extern int unpaidBills;
extern float fundBalance;

#endif
