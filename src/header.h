#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure to store date information
struct Date {
    int month, day, year;
};

// Structure to store account record information
struct Record {
    int id;
    int userId;
    char name[100];
    char country[100];
    int phone;
    char accountType[10];
    int accountNbr;
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

// Structure to store user information
struct User {
    int id;
    char name[50];
    char password[50];
};

// Authentication functions
void loginMenu(char a[50], char pass[50]); // Function to handle user login
void registerMenu(char a[50], char pass[50]); // Function to handle user registration
const char *getPassword(struct User u); // Function to get the password of a user

// System functions
void createNewAcc(struct User u); // Function to create a new account
void mainMenu(struct User u); // Function to display the main menu
void checkAllAccounts(struct User u); // Function to check all accounts
void updateAccountInfo(struct User u); // Function to update account information
void checkAccounts(struct User u); // Function to check specific accounts
void makeTransaction(struct User u); // Function to make a transaction
void removeAccount(struct User u); // Function to remove an account
void transferOwnership(struct User u); // Function to transfer account ownership

// Utility functions
void success(struct User u); // Function to display success message
int getAccountFromFile(FILE *pf, char *userName, struct Record *r); // Function to get account from file
void saveAccountToFile(FILE *pf, struct User u, struct Record r); // Function to save account to file
int get_int_input(int min_value, int max_value); // Function to get integer input within a range
void getCurrentDate(struct Date *date); // Function to get the current date
int getch(); // Function to wait for a key press