#define _CRT_SECURE_NO_WARNINGS
#ifndef VALIDATION_H
#define VALIDATION_H

/* Input & Terminal Helpers */
void clearInputBuffer(void);
void pauseAndClear(void);
void enableConsoleColors(void);

/* Validated Input Readers */
int getValidInt(int min, int max);
float getValidFloat(float min, float max);
void getValidString(char str[], int size);

/* Domain Validators */
int isValidName(const char name[]);
int isValidAge(int age);
int isValidID(int id);
int isValidPhone(const char phone[]);
int isValidBloodGroup(const char bloodGroup[]);
int isValidGender(char gender);
int isValidSeverity(int severity);
int isValidQuantity(int quantity);
int isValidPrice(float price);
int isValidExpiry(int expiry);
int isValidPin(const char pin[]);

#endif

