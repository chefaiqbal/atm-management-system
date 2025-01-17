#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_USERS 1000
#define MAX_ACCOUNTS 10000
#define MAX_NAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100
#define MAX_COUNTRY_LENGTH 50
#define MAX_PHONE_LENGTH 20
#define MAX_TYPE_LENGTH 20
#define MAX_TRANSACTION_TYPE_LENGTH 20

struct Date {
    int year;
    int month;
    int day;
};

struct User {
    int id; // Primary key
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
};

struct Account {
    int id; // Primary key
    int user_id;
    char user_name[MAX_NAME_LENGTH];
    struct Date creationDate;
    char country[MAX_COUNTRY_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    double balance;
    char type_of_account[MAX_TYPE_LENGTH];
};

struct Transaction {
    int id; // Primary key
    int account_id;
    char type[MAX_TRANSACTION_TYPE_LENGTH]; // "deposit", "withdraw", or "interest"
    double amount;
    char date[11]; // Format: YYYY-MM-DD
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
int loadUserById(int id, struct User* user);
int saveAccount(struct Account* account);
int loadAccount(int id, struct Account* account);
int updateAccount(struct Account* account);
int deleteAccount(int id);
int authenticateUser(const char* name, const char* password, struct User* user);

// Transaction operations
int saveTransaction(struct Transaction* transaction);
void viewTransactions(int account_id);

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

// Declare calculateInterestRate
double calculateInterestRate(const char* accountType);

// System operations
void getCurrentDate(struct Date* date);
void success(struct User* u);
int getch(void);
float getInterestRate(const char* accountType);

// Initialization and database
void ensureDataDirectoryExists(void);
int initDatabase(void);
int closeDatabase(void);

// Helper functions
int isNumber(const char* str);
int isValidAccountType(const char* type);

// Add these function declarations
int updateAccount(struct Account* account);
int deleteAccount(int id);
void getCurrentDateStr(char* dateStr, size_t size);

#endif // HEADER_H