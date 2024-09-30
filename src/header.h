#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_USERS 100
#define MAX_ACCOUNTS 1000
#define MAX_NAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_COUNTRY_LENGTH 50
#define MAX_PHONE_LENGTH 20
#define MAX_ACCOUNT_TYPE_LENGTH 20

struct Date {
    int day;
    int month;
    int year;
};

struct User {
    int id;
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
};

struct Account {
    int id;
    int userId;
    char userName[MAX_NAME_LENGTH];
    int accountId;
    struct Date creationDate;
    char country[MAX_COUNTRY_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    double balance;
    char accountType[MAX_ACCOUNT_TYPE_LENGTH];
};

// Function prototypes
void mainMenu(void);
void loginMenu(void);
void registerUser(void);
void userMenu(struct User* user);
void createNewAccount(struct User* user);
void checkAccountDetails(struct User* user);
void updateAccountInfo(struct User* user);
void removeAccount(struct User* user);
void checkOwnedAccounts(struct User* user);
void makeTransaction(struct User* user);
void transferOwnership(struct User* user);
void saveUsers(void);
void loadUsers(void);
void saveAccounts(void);
void loadAccounts(void);
int findUser(const char* name);
int findAccount(int accountId);
double calculateInterest(struct Account* account);

// Global variables (to be defined in main.c)
extern struct User users[MAX_USERS];
extern int userCount;
extern struct Account accounts[MAX_ACCOUNTS];
extern int accountCount;

#endif // HEADER_H