#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Forward declarations for helper functions
extern sqlite3* db;

// Helper function to validate phone number and account type
int isNumber(const char* str);
int isValidAccountType(const char* type);

// Declare calculateInterestRate as extern if used
extern double calculateInterestRate(const char* accountType);

// Add this line here
extern int updateInterestTransaction(int account_id, const char* date, double new_amount);

// Function to calculate future total interest for fixed accounts
double getTotalExpectedInterest(struct Account account) {
    int durationYears = 0;
    
    if (strcmp(account.type_of_account, "fixed01") == 0) {
        durationYears = 1;
    } else if (strcmp(account.type_of_account, "fixed02") == 0) {
        durationYears = 2;
    } else if (strcmp(account.type_of_account, "fixed03") == 0) {
        durationYears = 3;
    } else {
        return 0.0; // No interest for current or unknown account types
    }
    
    double interestRate = calculateInterestRate(account.type_of_account);
    return account.balance * interestRate * durationYears;
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
    while (scanf("%d", &accountId) != 1 || accountId <= 0) {
        printf("Invalid account ID. Please enter a positive integer: ");
        // Clear invalid input
        while (getchar() != '\n');
    }
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == SQLITE_OK && account.user_id == user->id) {
        printf("\nAccount Details:\n");
        printf("Account ID: %d\n", account.id);
        printf("User Name: %s\n", account.user_name);
        printf("Date of Creation: %04d-%02d-%02d\n", account.creationDate.year, account.creationDate.month, account.creationDate.day); // Format: YYYY-MM-DD
        printf("Country: %s\n", account.country);
        printf("Phone Number: %s\n", account.phone);
        printf("Balance: $%.2f\n", account.balance);
        printf("Type of Account: %s\n", account.type_of_account);

        // Display interest information based on account type
        if (strcmp(account.type_of_account, "savings") == 0) {
            double monthlyInterest = account.balance * calculateInterestRate(account.type_of_account) / 12;
            printf("You will earn $%.2f of interest on the same day every month.\n", monthlyInterest);
        } else if (strcmp(account.type_of_account, "current") == 0) {
            printf("You will not earn interest because the account type is current.\n");
        } else {
            double totalInterest = getTotalExpectedInterest(account);
            if (totalInterest > 0.0) {
                int durationYears = 0;
                if (strcmp(account.type_of_account, "fixed01") == 0) {
                    durationYears = 1;
                } else if (strcmp(account.type_of_account, "fixed02") == 0) {
                    durationYears = 2;
                } else if (strcmp(account.type_of_account, "fixed03") == 0) {
                    durationYears = 3;
                }
                printf("You will earn a total of $%.2f of interest after %d year(s) from the date of deposit.\n", totalInterest, durationYears);
            }
        }

        // Display transactions
        printf("\nTransactions for Account ID %d:\n", account.id);
        printf("Type\tAmount\t\tDate\n");
        viewTransactions(account.id);

    } else {
        printf("Account not found or you don't have permission to view it.\n");
    }
    success(user);
}

// Make a transaction (e.g., deposit or withdrawal)
void makeTransaction(struct User* user) {
    int accountId;
    char transactionType[MAX_TRANSACTION_TYPE_LENGTH];
    double amount;

    printf("Enter the account ID for the transaction: ");
    while (scanf("%d", &accountId) != 1 || accountId <= 0) {
        printf("Invalid account ID. Please enter a positive integer: ");
        // Clear invalid input
        while (getchar() != '\n');
    }
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        // Restrict transactions for fixed accounts
        if (strcmp(account.type_of_account, "fixed01") == 0 ||
            strcmp(account.type_of_account, "fixed02") == 0 ||
            strcmp(account.type_of_account, "fixed03") == 0) {
            printf("Transactions are not allowed on fixed accounts.\n");
            success(user);
            return;
        }

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

        // Update the account balance in the database
        if (updateAccount(&account) == SQLITE_OK) {
            // Log the transaction
            struct Transaction transaction;
            transaction.account_id = account.id;
            strncpy(transaction.type, transactionType, MAX_TRANSACTION_TYPE_LENGTH - 1);
            transaction.type[MAX_TRANSACTION_TYPE_LENGTH - 1] = '\0'; // Ensure null-termination
            transaction.amount = amount;

            char currentDateStr[11];
            getCurrentDateStr(currentDateStr, sizeof(currentDateStr));
            strncpy(transaction.date, currentDateStr, sizeof(transaction.date) - 1);
            transaction.date[sizeof(transaction.date) - 1] = '\0'; // Ensure null-termination

            if (saveTransaction(&transaction) == SQLITE_OK) {
                printf("Transaction recorded successfully.\n");
            } else {
                printf("Failed to record transaction.\n");
            }

            printf("Transaction completed successfully.\n");
        } else {
            printf("Failed to complete transaction. Please try again.\n");
        }
    } else {
        printf("Account not found or you don't have permission to transact on it.\n");
    }
    success(user);
}

// Transfer ownership of an account
void transferOwnership(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to transfer: ");
    while (scanf("%d", &accountId) != 1 || accountId <= 0) {
        printf("Invalid account ID. Please enter a positive integer: ");
        // Clear invalid input
        while (getchar() != '\n');
    }
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

// Update account information
void updateAccountInfo(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to update: ");
    while (scanf("%d", &accountId) != 1 || accountId <= 0) {
        printf("Invalid account ID. Please enter a positive integer: ");
        // Clear invalid input
        while (getchar() != '\n');
    }
    getchar(); // Consume newline

    struct Account account;
    if (loadAccount(accountId, &account) == 0 && account.user_id == user->id) {
        int choice;
        printf("What would you like to update?\n");
        printf("1. Phone Number\n");
        printf("2. Country\n");
        printf("Enter your choice (1 or 2): ");
        while (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
            printf("Invalid choice. Please enter 1 or 2: ");
            // Clear invalid input
            while (getchar() != '\n');
        }
        getchar(); // Consume newline

        if (choice == 1) {
            // Update Phone Number
            while (1) {
                printf("Enter new phone number (digits only): ");
                scanf("%s", account.phone);
                getchar(); // Consume newline
                if (isNumber(account.phone)) {
                    break;
                } else {
                    printf("Invalid phone number. Please enter digits only.\n");
                }
            }
        } else if (choice == 2) {
            // Update Country
            printf("Enter new country (current: %s): ", account.country);
            scanf("%s", account.country);
            getchar(); // Consume newline
        }

        // Update the account in the database
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

// Add this new function to src/atm_operations.c
void updateInterestForAccount(struct User* user) {
    int account_id;
    char transaction_date[11];
    double correct_interest;

    printf("Enter the account ID to update interest: ");
    scanf("%d", &account_id);
    getchar(); // Consume newline

    printf("Enter the date of the transaction to update (YYYY-MM-DD): ");
    scanf("%10s", transaction_date);
    getchar(); // Consume newline

    printf("Enter the correct interest amount: ");
    scanf("%lf", &correct_interest);
    getchar(); // Consume newline

    int result = updateInterestTransaction(account_id, transaction_date, correct_interest);
    if (result == SQLITE_OK) {
        printf("Interest transaction updated successfully.\n");
    } else {
        printf("Failed to update interest transaction.\n");
    }
    success(user);
}