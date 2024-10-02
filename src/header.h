#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

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

// Global variables
extern struct User users[MAX_USERS];
extern int userCount;
extern struct Account accounts[MAX_ACCOUNTS];
extern int accountCount;
extern sqlite3* db;

// Database operations
int saveUser(struct User* user);
int loadUser(const char* name, struct User* user);
int loadUserById(int id, struct User* user); // **Added**
int saveAccount(struct Account* account);
int loadAccount(int accountId, struct Account* account);
int updateAccount(struct Account* account);
int deleteAccount(int accountId);
int authenticateUser(const char* name, const char* password, struct User* user);

// Menu operations
void mainMenu(void);
void loginMenu(void);
void registerUser(void);
void userMenu(struct User* user);

// ATM operations
void createNewAccount(struct User* user);
void checkAccountDetails(struct User* user);
void makeTransaction(struct User* user);
void updateAccountInfo(struct User* user);
void removeAccount(struct User* user);
void transferOwnership(struct User* user);
void checkOwnedAccounts(struct User* user);
double calculateInterest(const struct Account* account);

// System operations
void getCurrentDate(struct Date* date);
void success(struct User* u);
int getch(void);
float getInterestRate(const char* accountType);

// Initialization and database
void ensureDataDirectoryExists(void);
int initDatabase(void);
int closeDatabase(void);

#endif // HEADER_H