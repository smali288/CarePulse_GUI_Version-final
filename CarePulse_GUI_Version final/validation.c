#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "structs.h"
#include "validation.h"

/* Enables ANSI color escape codes on Windows 10/11 terminals */
void enableConsoleColors(void)
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

/* Waits for user to press Enter, then clears the terminal screen */
void pauseAndClear(void)
{
    printf("\n\033[1;36mPress [ENTER] to refresh and return to menu...\033[0m");
    fflush(stdout);
    clearInputBuffer();
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Clears all remaining characters from the current input line */
void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

/* Reads an integer and verifies it is within [min, max] */
int getValidInt(int min, int max)
{
    int number;
    int ch;
    int valid;

    while (1) {
        valid = 1;

        if (scanf("%d", &number) != 1) {
            printf("\033[1;31mInvalid input.\033[0m Enter a whole number: ");
            clearInputBuffer();
            continue;
        }

        ch = getchar();
        while (ch != '\n' && ch != EOF) {
            if (!isspace((unsigned char)ch)) {
                valid = 0;
            }
            ch = getchar();
        }

        if (!valid) {
            printf("\033[1;31mInvalid input.\033[0m Enter a whole number: ");
            continue;
        }

        if (number < min || number > max) {
            printf("\033[1;33mOut of range.\033[0m Enter a number between %d and %d: ", min, max);
            continue;
        }

        return number;
    }
}

/* Reads a floating-point number and verifies it is within [min, max] */
float getValidFloat(float min, float max)
{
    float number;
    int ch;
    int valid;

    while (1) {
        valid = 1;

        if (scanf("%f", &number) != 1) {
            printf("\033[1;31mInvalid input.\033[0m Enter a valid amount: ");
            clearInputBuffer();
            continue;
        }

        ch = getchar();
        while (ch != '\n' && ch != EOF) {
            if (!isspace((unsigned char)ch)) {
                valid = 0;
            }
            ch = getchar();
        }

        if (!valid) {
            printf("\033[1;31mInvalid input.\033[0m Enter a valid amount: ");
            continue;
        }

        if (number < min || number > max) {
            printf("\033[1;33mOut of range.\033[0m Enter an amount between %.2f and %.2f: ", min, max);
            continue;
        }

        return number;
    }
}

/* Reads a non-empty string safely within size */
void getValidString(char str[], int size)
{
    char temp[128];

    while (1) {
        if (fgets(temp, sizeof(temp), stdin) == NULL) {
            str[0] = '\0';
            return;
        }

        if (strchr(temp, '\n') == NULL) {
            clearInputBuffer();
            printf("\033[1;33mInput is too long.\033[0m Try again: ");
            continue;
        }

        temp[strcspn(temp, "\n")] = '\0';

        if (strlen(temp) == 0) {
            printf("\033[1;33mInput cannot be empty.\033[0m Try again: ");
            continue;
        }

        if ((int)strlen(temp) >= size) {
            printf("\033[1;33mMaximum allowed length is %d characters.\033[0m Try again: ", size - 1);
            continue;
        }

        strcpy(str, temp);
        return;
    }
}

int isValidName(const char name[])
{
    int i;
    if (name == NULL || strlen(name) == 0 || strlen(name) >= MAX_NAME) {
        return 0;
    }
    for (i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

int isValidAge(int age)
{
    return age >= 0 && age <= 120;
}

int isValidID(int id)
{
    return id > 0;
}

int isValidPhone(const char phone[])
{
    int i;
    if (phone == NULL || strlen(phone) != 11) {
        return 0;
    }
    for (i = 0; phone[i] != '\0'; i++) {
        if (!isdigit((unsigned char)phone[i])) {
            return 0;
        }
    }
    return 1;
}

int isValidBloodGroup(const char bloodGroup[])
{
    const char* validGroups[] = {
        "A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"
    };
    int i;
    if (bloodGroup == NULL) {
        return 0;
    }
    for (i = 0; i < 8; i++) {
        if (strcmp(bloodGroup, validGroups[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isValidGender(char gender)
{
    gender = (char)toupper((unsigned char)gender);
    return gender == 'M' || gender == 'F' || gender == 'O';
}

int isValidSeverity(int severity)
{
    return severity >= 1 && severity <= 5;
}

int isValidQuantity(int quantity)
{
    return quantity >= 0;
}

int isValidPrice(float price)
{
    return price > 0.0f;
}

int isValidExpiry(int expiry)
{
    int year = expiry / 10000;
    int month = (expiry / 100) % 100;
    int day = expiry % 100;

    if (year < 2000 || year > 2099) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    return 1;
}

int isValidPin(const char pin[])
{
    int i;
    if (pin == NULL || strlen(pin) != 4) {
        return 0;
    }
    for (i = 0; pin[i] != '\0'; i++) {
        if (!isdigit((unsigned char)pin[i])) {
            return 0;
        }
    }
    return 1;
}
