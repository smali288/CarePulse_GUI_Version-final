#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <conio.h>
#endif
#include "security.h"
#include "validation.h"
#include "structs.h"

#define MAX_PASS_LEN 10

static char adminPin[PIN_LEN] = DEFAULT_PIN;

/* Validates Full Name: letters/spaces only AND requires first + last name */
static int isValidFullStaffName(const char name[])
{
    int i = 0;
    int wordCount = 0;
    int inWord = 0;

    if (!isValidName(name)) {
        return 0;
    }

    while (name[i] != '\0') {
        if (isalpha((unsigned char)name[i])) {
            if (!inWord) {
                wordCount++;
                inWord = 1;
            }
        }
        else if (name[i] == ' ') {
            inWord = 0;
        }
        i++;
    }

    return wordCount >= 2;
}

/* Validates Username: no spaces, alphanumeric/underscore only, must contain at least 1 letter */
static int isValidStaffUsername(const char user[])
{
    int i;
    int hasLetter = 0;

    if (user == NULL || strlen(user) < 3) {
        return 0;
    }

    for (i = 0; user[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)user[i];
        if (isalpha(ch)) {
            hasLetter = 1;
        }
        else if (!isdigit(ch) && ch != '_') {
            return 0;
        }
    }

    return hasLetter;
}

/* Reads masked input (****) and strictly blocks typing beyond maxChars */
static void readMaskedInput(char buffer[], int bufSize, int maxChars)
{
#ifdef _WIN32
    int idx = 0;
    int ch;
    int limit = (maxChars < bufSize - 1) ? maxChars : (bufSize - 1);

    while (1) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') {
            buffer[idx] = '\0';
            printf("\n");
            break;
        }
        else if (ch == '\b') {
            if (idx > 0) {
                idx--;
                printf("\b \b");
            }
        }
        else if (ch >= 32 && ch <= 126) {
            if (idx < limit) {
                buffer[idx++] = (char)ch;
                printf("*");
            }
            else {
                printf("\a");
            }
        }
    }
#else
    getValidString(buffer, bufSize);
    if ((int)strlen(buffer) > maxChars) {
        buffer[maxChars] = '\0';
    }
#endif
}

int verifyAdminPin(void)
{
    char enteredPin[PIN_LEN];
    int attempts = 0;

    while (attempts < MAX_ATTEMPTS) {
        printf("Enter admin PIN (default 1234): ");
        readMaskedInput(enteredPin, sizeof(enteredPin), 4);

        if (isValidPin(enteredPin) && strcmp(enteredPin, adminPin) == 0) {
            printf("\033[1;32mAccess granted.\033[0m\n");
            return 1;
        }

        attempts++;
        if (attempts < MAX_ATTEMPTS) {
            printf("\033[1;31mIncorrect PIN.\033[0m Attempts remaining: %d\n",
                MAX_ATTEMPTS - attempts);
        }
    }

    printf("\033[1;31mToo many incorrect attempts. Admin access locked for this session.\033[0m\n");
    return 0;
}

void changePin(void)
{
    char oldPin[PIN_LEN];
    char newPin[PIN_LEN];
    char confirmPin[PIN_LEN];

    printf("Enter current PIN: ");
    readMaskedInput(oldPin, sizeof(oldPin), 4);

    if (!isValidPin(oldPin) || strcmp(oldPin, adminPin) != 0) {
        printf("\033[1;31mIncorrect current PIN.\033[0m\n");
        return;
    }

    printf("Enter new 4-digit PIN: ");
    readMaskedInput(newPin, sizeof(newPin), 4);

    if (!isValidPin(newPin)) {
        printf("\033[1;31mPIN must contain exactly 4 digits.\033[0m\n");
        return;
    }

    printf("Confirm new PIN: ");
    readMaskedInput(confirmPin, sizeof(confirmPin), 4);

    if (strcmp(newPin, confirmPin) != 0) {
        printf("\033[1;31mPIN confirmation does not match.\033[0m\n");
        return;
    }

    strcpy(adminPin, newPin);
    printf("\033[1;32mPIN changed successfully for this session.\033[0m\n");
}

/* Persistent Account System saved in users.txt (Max 3 login attempts, max 10-char password, '0' to go back) */
int userLoginPortal(char loggedInUser[], char loggedInRole[])
{
    int choice;
    int loginAttempts = 0;
    char username[MAX_NAME];
    char password[MAX_NAME];
    char fullName[MAX_NAME];
    char role[20];
    char fileUser[MAX_NAME] = { 0 };
    char filePass[MAX_NAME] = { 0 };
    char fileName[MAX_NAME] = { 0 };
    char fileRole[20] = { 0 };

    while (1) {
        printf("\033[1;36m====================================================\n");
        printf("      HOSPITAL PLATFORM - STAFF LOGIN PORTAL\n");
        printf("====================================================\033[0m\n");
        printf("1. Login to Existing Account (%d attempts remaining)\n", MAX_ATTEMPTS - loginAttempts);
        printf("2. Create New Staff Account (Saved Permanently)\n");
        printf("3. Exit Program\n");
        printf("Enter choice (1-3): ");
        choice = getValidInt(1, 3);

        if (choice == 3) {
            return 0;
        }
        else if (choice == 2) {
            FILE* fp;
            int exists = 0;
            int r;

            printf("\n--- CREATE NEW STAFF ACCOUNT (Type '0' anytime to go back) ---\n");

            /* Step 1: Full Name */
            while (1) {
                printf("Enter Full Name (First & Last Name, or 0 to cancel): ");
                getValidString(fullName, sizeof(fullName));
                if (strcmp(fullName, "0") == 0) {
                    printf("\033[1;33mAccount creation cancelled. Returning to portal...\033[0m\n");
                    pauseAndClear();
                    break;
                }
                if (isValidFullStaffName(fullName)) {
                    break;
                }
                printf("\033[1;31mError: Enter both First and Last name using letters only.\033[0m\n");
            }
            if (strcmp(fullName, "0") == 0) continue;

            /* Step 2: Username */
            while (1) {
                printf("Choose Username (no spaces, must include letters, or 0 to go back): ");
                getValidString(username, sizeof(username));
                if (strcmp(username, "0") == 0) {
                    printf("\033[1;33mGoing back to portal...\033[0m\n");
                    pauseAndClear();
                    break;
                }
                if (isValidStaffUsername(username)) {
                    break;
                }
                printf("\033[1;31mError: Username must be at least 3 characters, have no spaces, and contain at least one letter.\033[0m\n");
            }
            if (strcmp(username, "0") == 0) continue;

            fp = fopen("users.txt", "r");
            if (fp != NULL) {
                while (fscanf(fp, "%30[^|]|%30[^|]|%30[^|]|%19[^\n]\n",
                    fileUser, filePass, fileName, fileRole) == 4) {
                    if (strcmp(fileUser, username) == 0) {
                        exists = 1;
                        break;
                    }
                }
                fclose(fp);
            }

            if (exists) {
                printf("\033[1;31mUsername already exists! Try logging in.\033[0m\n");
                pauseAndClear();
                continue;
            }

            /* Step 3: Role */
            printf("Select Role (1 = Admin, 2 = Pharmacist, 3 = Dispatcher, 0 = Go Back): ");
            r = getValidInt(0, 3);
            if (r == 0) {
                printf("\033[1;33mGoing back to portal...\033[0m\n");
                pauseAndClear();
                continue;
            }
            if (r == 1) strcpy(role, "Admin");
            else if (r == 2) strcpy(role, "Pharmacist");
            else strcpy(role, "Dispatcher");

            /* Step 4: Password (Strictly max 10 chars while typing) */
            while (1) {
                printf("Create Password (max %d characters, or 0 to go back): ", MAX_PASS_LEN);
                readMaskedInput(password, sizeof(password), MAX_PASS_LEN);

                if (strcmp(password, "0") == 0) {
                    printf("\033[1;33mGoing back to portal...\033[0m\n");
                    pauseAndClear();
                    break;
                }
                if (strlen(password) == 0) {
                    printf("\033[1;31mPassword cannot be empty.\033[0m\n");
                }
                else {
                    break;
                }
            }
            if (strcmp(password, "0") == 0) continue;

            fp = fopen("users.txt", "a");
            if (fp != NULL) {
                fprintf(fp, "%s|%s|%s|%s\n", username, password, fullName, role);
                fclose(fp);
                printf("\033[1;32mAccount created and saved permanently! You can now log in.\033[0m\n");
            }
            else {
                printf("\033[1;31mError saving account file.\033[0m\n");
            }
            pauseAndClear();
        }
        else if (choice == 1) {
            FILE* fp = fopen("users.txt", "r");
            int found = 0;

            if (fp == NULL) {
                printf("\033[1;33mNo accounts exist yet. Please select Option 2 to create an account first.\033[0m\n");
                pauseAndClear();
                continue;
            }

            printf("\nUsername (or 0 to go back): ");
            getValidString(username, sizeof(username));
            if (strcmp(username, "0") == 0) {
                fclose(fp);
                pauseAndClear();
                continue;
            }

            printf("Password (max %d chars, or 0 to go back): ", MAX_PASS_LEN);
            readMaskedInput(password, sizeof(password), MAX_PASS_LEN);
            if (strcmp(password, "0") == 0) {
                fclose(fp);
                pauseAndClear();
                continue;
            }

            while (fscanf(fp, "%30[^|]|%30[^|]|%30[^|]|%19[^\n]\n",
                fileUser, filePass, fileName, fileRole) == 4) {
                if (strcmp(fileUser, username) == 0 && strcmp(filePass, password) == 0) {
                    strcpy(loggedInUser, fileName);
                    strcpy(loggedInRole, fileRole);
                    found = 1;
                    break;
                }
            }
            fclose(fp);
            if (found) {
                printf("\033[1;32mWelcome back, %s (%s)!\033[0m\n", loggedInUser, loggedInRole);
                pauseAndClear();
                return 1;
            }
            else {
                loginAttempts++;
                if (loginAttempts >= MAX_ATTEMPTS) {
                    printf("\033[1;31mMaximum 3 login attempts exceeded. System locked for security!\033[0m\n");
                    pauseAndClear();
                    return 0;
                }
                printf("\033[1;31mInvalid username or password.\033[0m Attempts remaining: %d\n",
                    MAX_ATTEMPTS - loginAttempts);
                pauseAndClear();
            }
}
}
}