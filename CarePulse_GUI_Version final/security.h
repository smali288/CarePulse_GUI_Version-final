#define _CRT_SECURE_NO_WARNINGS
#ifndef SECURITY_H
#define SECURITY_H

int verifyAdminPin(void);
void changePin(void);

/* Persistent Account Login & Registration Gate */
int userLoginPortal(char loggedInUser[], char loggedInRole[]);

#endif
