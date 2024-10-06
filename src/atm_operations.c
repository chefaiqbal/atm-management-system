#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>

extern sqlite3* db;

// Helper function to check if a string contains only digits
int isNumber(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function to validate account type
int isValidAccountType(const char* type) {
    const char* validTypes[] = {"current", "savings", "fixed01", "fixed02", "fixed03"};
    size_t numTypes = sizeof(validTypes) / sizeof(validTypes[0]);
    for (size_t i = 0; i < numTypes; i++) {
        if (strcmp(type, validTypes[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Create a new account for the user
void createNewAccount(struct User* user) {
    struct Account newAccount;
    newAccount.user_id = user->id;
    strncpy(newAccount.user_name, user->name, MAX_NAME_LENGTH - 1);
    newAccount.user_name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination

    getCurrentDate(&newAccount.creationDate);

    // Input: Country
    printf("Enter country: ");
    scanf("%s", newAccount.country);
    getchar(); // Consume newline

    // Input: Phone Number with validation
    while (1) {
        printf("Enter phone number (digits only): ");
        scanf("%s", newAccount.phone);
        getchar(); // Consume newline
        if (isNumber(newAccount.phone)) {
            break;
        } else {
            printf("Invalid phone number. Please enter digits only.\n");
        }
    }

    // Input: Initial Balance
    printf("Enter initial balance: ");
    while (scanf("%lf", &newAccount.balance) != 1 || newAccount.balance < 0) {
        printf("Invalid amount. Please enter a positive number: ");
        // Clear invalid input
        while (getchar() != '\n');
    }
    getchar(); // Consume newline

    // Input: Account Type with validation
    while (1) {
        printf("Enter account type (current/savings/fixed01/fixed02/fixed03): ");
        scanf("%s", newAccount.type_of_account);
        getchar(); // Consume newline
        if (isValidAccountType(newAccount.type_of_account)) {
            break;
        } else {
            printf("Invalid account type. Please choose from the listed options.\n");
        }
    }

    if (saveAccount(&newAccount) == SQLITE_OK) {
        printf("Account created successfully. Your account ID is: %d\n", newAccount.id);
    } else {
        printf("Failed to create account. Please try again.\n");
    }
    success(user);
}

// Check details of a specific account
void checkAccountDetails(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to check: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        printf("\nAccount Details:\n");
        printf("Account ID: %d\n", account.id);
        printf("User Name: %s\n", account.user_name);
        printf("Creation Date: %02d/%02d/%04d\n", account.creationDate.day, account.creationDate.month, account.creationDate.year);
        printf("Country: %s\n", account.country);
        printf("Phone Number: %s\n", account.phone);
        printf("Balance: $%.2f\n", account.balance);
        printf("Account Type: %s\n", account.type_of_account);
    } else {
        printf("Account not found or you don't have permission to view it.\n");
    }
    success(user);
}

// Transfer ownership of an account to another user
void transferOwnership(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to transfer: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        char newUserName[MAX_NAME_LENGTH];
        printf("Enter the username of the new owner: ");
        scanf("%s", newUserName);
        getchar(); // Consume newline

        struct User newUser;
        if (loadUser(newUserName, &newUser) == 0) {
            account.user_id = newUser.id;
            strncpy(account.user_name, newUser.name, MAX_NAME_LENGTH - 1);
            account.user_name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination

            if (updateAccount(&account) == SQLITE_OK) {
                printf("Ownership transferred successfully.\n");
            } else {
                printf("Failed to transfer ownership. Please try again.\n");
            }
        } else {
            printf("New user not found.\n");
        }
    } else {
        printf("Account not found or you don't have permission to transfer it.\n");
    }
    success(user);
}

// Check all accounts owned by the user
void checkOwnedAccounts(struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, type_of_account, balance FROM accounts WHERE user_id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in checkOwnedAccounts: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, user->id);

    printf("\nYour Accounts:\n");
    printf("ID\tType\t\tBalance\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int accountId = sqlite3_column_int(stmt, 0);
        const char* accountType = (const char*)sqlite3_column_text(stmt, 1);
        double balance = sqlite3_column_double(stmt, 2);
        printf("%d\t%-10s\t$%.2f\n", accountId, accountType, balance);
    }

    sqlite3_finalize(stmt);
    success(user);
}

// Calculate monthly interest based on account type
double calculateInterest(const struct Account* account) {
    double interestRate = getInterestRate(account->type_of_account);
    return (interestRate > 0) ? (account->balance * interestRate / 12) : 0.0;
}

// Update account information
void updateAccountInfo(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to update: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        printf("Enter new phone number (current: %s): ", account.phone);
        scanf("%s", account.phone);
        getchar(); // Consume newline
        
        printf("Enter new country (current: %s): ", account.country);
        scanf("%s", account.country);
        getchar(); // Consume newline
        
        printf("Enter new account type (current: %s): ", account.type_of_account);
        scanf("%s", account.type_of_account);
        getchar(); // Consume newline

        if (saveAccount(&account) == SQLITE_OK) {
            printf("Account information updated successfully.\n");
        } else {
            printf("Failed to update account information. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to update it.\n");
    }
    success(user);
}

// Remove an account
void removeAccount(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to remove: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        if (deleteAccount(accountId) == SQLITE_OK) {
            printf("Account removed successfully.\n");
        } else {
            printf("Failed to remove account. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to remove it.\n");
    }
    success(user);
}

// Make a transaction (e.g., deposit or withdrawal)
void makeTransaction(struct User* user) {
    int accountId;
    char transactionType[10];
    double amount;

    // Input: Account ID
    printf("Enter the account ID for the transaction: ");
    while (scanf("%d", &accountId) != 1 || accountId <= 0) {
        printf("Invalid account ID. Please enter a positive integer: ");
        // Clear invalid input
        while (getchar() != '\n');
    }
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        // Input: Transaction Type with validation
        while (1) {
            printf("Enter transaction type (deposit/withdraw): ");
            scanf("%s", transactionType);
            getchar(); // Consume newline
            if (strcmp(transactionType, "deposit") == 0 || strcmp(transactionType, "withdraw") == 0) {
                break;
            } else {
                printf("Invalid transaction type. Please enter 'deposit' or 'withdraw'.\n");
            }
        }

        // Input: Amount with validation
        printf("Enter amount: ");
        while (scanf("%lf", &amount) != 1 || amount <= 0) {
            printf("Invalid amount. Please enter a positive number: ");
            // Clear invalid input
            while (getchar() != '\n');
        }
        getchar(); // Consume newline

        if (strcmp(transactionType, "deposit") == 0) {
            account.balance += amount;
            printf("Deposited $%.2f successfully.\n", amount);
        } else if (strcmp(transactionType, "withdraw") == 0) {
            if (account.balance >= amount) {
                account.balance -= amount;
                printf("Withdrew $%.2f successfully.\n", amount);
            } else {
                printf("Insufficient balance for withdrawal.\n");
                success(user);
                return;
            }
        } else {
            printf("Invalid transaction type.\n");
            success(user);
            return;
        }

        if (saveAccount(&account) == SQLITE_OK) {
            printf("Transaction completed successfully.\n");
        } else {
            printf("Failed to complete transaction. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to transact on it.\n");
    }
    success(user);
}