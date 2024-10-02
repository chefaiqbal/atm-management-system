#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <limits.h>

extern sqlite3* db;

// Create a new account for the user
void createNewAccount(struct User* user) {
    struct Account newAccount;
    newAccount.userId = user->id;
    strncpy(newAccount.userName, user->name, MAX_NAME_LENGTH);
    newAccount.accountId = rand() % 900000 + 100000; // Generate a random 6-digit account number

    getCurrentDate(&newAccount.creationDate);

    printf("Enter country: ");
    scanf("%s", newAccount.country);
    printf("Enter phone number: ");
    scanf("%s", newAccount.phone);
    printf("Enter initial balance: ");
    scanf("%lf", &newAccount.balance);
    printf("Enter account type (current/savings/fixed01/fixed02/fixed03): ");
    scanf("%s", newAccount.accountType);

    if (saveAccount(&newAccount) == SQLITE_OK) {
        printf("Account created successfully. Your account ID is: %d\n", newAccount.accountId);
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

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.userId == user->id) {
        printf("\nAccount Details:\n");
        printf("Account ID: %d\n", account.accountId);
        printf("User Name: %s\n", account.userName);
        printf("Creation Date: %02d/%02d/%04d\n", account.creationDate.day, account.creationDate.month, account.creationDate.year);
        printf("Country: %s\n", account.country);
        printf("Phone: %s\n", account.phone);
        printf("Balance: $%.2f\n", account.balance);
        printf("Account Type: %s\n", account.accountType);

        double interest = calculateInterest(&account);
        if (interest > 0) {
            printf("You will get $%.2f as interest on day %d of every month\n", interest, account.creationDate.day);
        } else {
            printf("You will not get interests because the account is of type current\n");
        }
    } else {
        printf("Account not found or you don't have permission to access it.\n");
    }
    success(user);
}

// Make a transaction (deposit/withdraw) on an account
void makeTransaction(struct User* user) {
    int accountId;
    printf("Enter the account ID for the transaction: ");
    scanf("%d", &accountId);

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.userId == user->id) {
        if (strncmp(account.accountType, "fixed", 5) == 0) {
            printf("Transactions are not allowed for fixed deposit accounts.\n");
            success(user);
            return;
        }

        int choice;
        double amount;
        printf("1. Deposit\n");
        printf("2. Withdraw\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("Enter the amount: $");
        scanf("%lf", &amount);

        switch (choice) {
            case 1:
                account.balance += amount;
                printf("Deposit successful. New balance: $%.2f\n", account.balance);
                break;
            case 2:
                if (amount > account.balance) {
                    printf("Insufficient funds. Withdrawal cancelled.\n");
                    success(user);
                    return;
                } else {
                    account.balance -= amount;
                    printf("Withdrawal successful. New balance: $%.2f\n", account.balance);
                }
                break;
            default:
                printf("Invalid choice. Transaction cancelled.\n");
                success(user);
                return;
        }

        if (updateAccount(&account) == SQLITE_OK) {
            printf("Transaction completed successfully.\n");
        } else {
            printf("Failed to complete transaction. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to access it.\n");
    }
    success(user);
}

// Update account information (country or phone number)
void updateAccountInfo(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to update: ");
    scanf("%d", &accountId);

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.userId == user->id) {
        int choice;
        printf("What do you want to update?\n");
        printf("1. Country\n");
        printf("2. Phone number\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter new country: ");
                scanf("%s", account.country);
                break;
            case 2:
                printf("Enter new phone number: ");
                scanf("%s", account.phone);
                break;
            default:
                printf("Invalid choice. Update cancelled.\n");
                success(user);
                return;
        }

        if (updateAccount(&account) == SQLITE_OK) {
            printf("Account information updated successfully.\n");
        } else {
            printf("Failed to update account information. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to update it.\n");
    }
    success(user);
}

// Remove an account from the system
void removeAccount(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to remove: ");
    scanf("%d", &accountId);

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.userId == user->id) {
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

// Transfer ownership of an account to another user
void transferOwnership(struct User* user) {
    int accountId, newUserId;
    printf("Enter the account ID you want to transfer: ");
    scanf("%d", &accountId);

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.userId == user->id) {
        printf("Enter the new user ID: ");
        scanf("%d", &newUserId);

        // Check if new user exists
        struct User newUser;
        if (loadUserById(newUserId, &newUser) != 0) { // Implement loadUserById if necessary
            printf("New user ID not found. Transfer cancelled.\n");
            success(user);
            return;
        }

        account.userId = newUserId;
        strncpy(account.userName, newUser.name, MAX_NAME_LENGTH);

        if (updateAccount(&account) == SQLITE_OK) {
            printf("Ownership transferred successfully.\n");
        } else {
            printf("Failed to transfer ownership. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to transfer it.\n");
    }
    success(user);
}

// Check all accounts owned by the user
void checkOwnedAccounts(struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT accountId, accountType, balance FROM accounts WHERE userId = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
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
    double interestRate = getInterestRate(account->accountType);
    return (interestRate > 0) ? (account->balance * interestRate / 12) : 0.0;
}