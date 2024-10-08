#include "header.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <openssl/sha.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

// Function declaration is already in header.h, so it's optional here
// double calculateInterestRate(const char* accountType);

// Helper function to hash a password using SHA256
void hashPassword(const char* password, char* hashedPassword) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, strlen(password), hash);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashedPassword + (i * 2), "%02x", hash[i]);
    }
    hashedPassword[SHA256_DIGEST_LENGTH * 2] = '\0';
}

// Helper function to get current date as string YYYY-MM-DD
void getCurrentDateStr(char* dateStr, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(dateStr, size, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); // YYYY-MM-DD format
}

// Save a new user to the database
int saveUser(struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (name, password) VALUES (?, ?);";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Hash the password before storing
    char hashedPassword[SHA256_DIGEST_LENGTH * 2 + 1];
    hashPassword(user->password, hashedPassword);
    
    sqlite3_bind_text(stmt, 1, user->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    } else {
        user->id = sqlite3_last_insert_rowid(db);
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Load a user by name
int loadUser(const char* name, struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name, password FROM users WHERE name = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in loadUser: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        strncpy(user->name, (const char*)sqlite3_column_text(stmt, 1), MAX_NAME_LENGTH - 1);
        user->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
        strncpy(user->password, (const char*)sqlite3_column_text(stmt, 2), MAX_PASSWORD_LENGTH - 1);
        user->password[MAX_PASSWORD_LENGTH - 1] = '\0'; // Ensure null-termination
        sqlite3_finalize(stmt);
        return 0; // Success
    }

    sqlite3_finalize(stmt);
    return 1; // User not found
}

// Load a user by ID
int loadUserById(int id, struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name, password FROM users WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in loadUserById: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        strncpy(user->name, (const char*)sqlite3_column_text(stmt, 1), MAX_NAME_LENGTH - 1);
        user->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
        strncpy(user->password, (const char*)sqlite3_column_text(stmt, 2), MAX_PASSWORD_LENGTH - 1);
        user->password[MAX_PASSWORD_LENGTH - 1] = '\0'; // Ensure null-termination
        sqlite3_finalize(stmt);
        return 0; // Success
    }

    sqlite3_finalize(stmt);
    return 1; // User not found
}

// Helper function to add years to a date and return a new date string in YYYY-MM-DD format
void calculateFutureDate(struct Date creationDate, int yearsToAdd, char* futureDateStr, size_t size) {
    int newYear = creationDate.year + yearsToAdd;
    snprintf(futureDateStr, size, "%04d-%02d-%02d", newYear, creationDate.month, creationDate.day);
}

// Implementation of calculateInterestRate
double calculateInterestRate(const char* accountType) {
    if (strcmp(accountType, "savings") == 0) {
        return 0.07; // 7% annual interest rate for savings accounts
    } else if (strcmp(accountType, "fixed01") == 0) {
        return 0.04; // 4% annual interest rate for 1-year fixed accounts
    } else if (strcmp(accountType, "fixed02") == 0) {
        return 0.05; // 5% annual interest rate for 2-year fixed accounts
    } else if (strcmp(accountType, "fixed03") == 0) {
        return 0.08; // 8% annual interest rate for 3-year fixed accounts
    } else if (strcmp(accountType, "current") == 0) {
        return 0.0; // No interest for current accounts
    } else {
        return 0.0; // Default to no interest for unknown account types
    }
}

// Helper function to schedule interest transactions
void scheduleInterest(struct Account* account) {
    if (strcmp(account->type_of_account, "savings") == 0) {
        // Schedule monthly interest on the same day as the account creation date
        struct Transaction interest;
        interest.account_id = account->id;
        strcpy(interest.type, "interest");

        // Calculate the first interest date (next month on the same day from creation date)
        struct Date nextInterestDate = account->creationDate;
        nextInterestDate.month += 1;
        if (nextInterestDate.month > 12) {
            nextInterestDate.month = 1;
            nextInterestDate.year += 1;
        }

        char interestDateStr[11]; // Format: YYYY-MM-DD
        snprintf(interestDateStr, sizeof(interestDateStr), "%04d-%02d-%02d",
                 nextInterestDate.year, nextInterestDate.month, nextInterestDate.day);
        strncpy(interest.date, interestDateStr, sizeof(interest.date) - 1);
        interest.date[sizeof(interest.date) - 1] = '\0'; // Ensure null-termination

        // Calculate the monthly interest amount dynamically
        double interestRate = calculateInterestRate(account->type_of_account);
        interest.amount = account->balance * interestRate / 12;

        if (saveTransaction(&interest) == SQLITE_OK) {
            printf("Interest transaction scheduled on %s.\n", interest.date);
        } else {
            printf("Failed to schedule interest transaction.\n");
        }
    } else {
        // Handle fixed account types dynamically
        int durationYears = 0;
        double interestRate = 0.0;
        double interestAmount = 0.0;

        if (strcmp(account->type_of_account, "fixed01") == 0) {
            durationYears = 1;
        } else if (strcmp(account->type_of_account, "fixed02") == 0) {
            durationYears = 2;
        } else if (strcmp(account->type_of_account, "fixed03") == 0) {
            durationYears = 3;
        } else if (strcmp(account->type_of_account, "current") == 0) {
            // Current accounts do not earn interest
            printf("You will not earn interest because the account is of type current.\n");
            return;
        } else {
            fprintf(stderr, "Unknown account type.\n");
            return;
        }

        // Get the interest rate based on account type
        interestRate = calculateInterestRate(account->type_of_account);

        // Calculate the total interest amount dynamically
        interestAmount = account->balance * interestRate * durationYears;

        // Calculate the due date by adding the duration to the creation date
        char dueDateStr[11]; // Format: YYYY-MM-DD
        struct Date dueDate = account->creationDate;
        dueDate.year += durationYears;
        snprintf(dueDateStr, sizeof(dueDateStr), "%04d-%02d-%02d",
                 dueDate.year, dueDate.month, dueDate.day);

        // Schedule the interest transaction on the due date
        struct Transaction interest;
        interest.account_id = account->id;
        strcpy(interest.type, "interest");
        interest.amount = interestAmount;
        strncpy(interest.date, dueDateStr, sizeof(interest.date) - 1);
        interest.date[sizeof(interest.date) - 1] = '\0'; // Ensure null-termination

        if (saveTransaction(&interest) == SQLITE_OK) {
            printf("Interest transaction scheduled on %s.\n", interest.date);
        } else {
            printf("Failed to schedule interest transaction.\n");
        }
    }
}

// Save a new account to the database
int saveAccount(struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO accounts (user_id, user_name, date_of_creation, country, phone, balance, type_of_account) VALUES (?, ?, ?, ?, ?, ?, ?);";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in saveAccount: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    char creationDateStr[11]; // Format: YYYY-MM-DD
    snprintf(creationDateStr, sizeof(creationDateStr), "%04d-%02d-%02d", account->creationDate.year, account->creationDate.month, account->creationDate.day); // YYYY-MM-DD format

    sqlite3_bind_int(stmt, 1, account->user_id);
    sqlite3_bind_text(stmt, 2, account->user_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, creationDateStr, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, account->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, account->phone, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, account->balance);
    sqlite3_bind_text(stmt, 7, account->type_of_account, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement in saveAccount: %s\n", sqlite3_errmsg(db));
    } else {
        account->id = sqlite3_last_insert_rowid(db);

        // Save initial deposit as a transaction
        struct Transaction initialDeposit;
        initialDeposit.account_id = account->id;
        strcpy(initialDeposit.type, "deposit");
        initialDeposit.amount = account->balance;
        strncpy(initialDeposit.date, creationDateStr, sizeof(initialDeposit.date) - 1); // Ensure YYYY-MM-DD format
        initialDeposit.date[sizeof(initialDeposit.date) - 1] = '\0'; // Ensure null-termination

        if (saveTransaction(&initialDeposit) == SQLITE_OK) {
            printf("Initial deposit transaction saved successfully.\n");
        } else {
            printf("Failed to save initial deposit transaction.\n");
        }

        // Schedule interest transactions
        scheduleInterest(account);
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Load an account by ID
int loadAccount(int id, struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, user_id, user_name, date_of_creation, country, phone, balance, type_of_account FROM accounts WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in loadAccount: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        account->id = sqlite3_column_int(stmt, 0);
        account->user_id = sqlite3_column_int(stmt, 1);
        strncpy(account->user_name, (const char*)sqlite3_column_text(stmt, 2), MAX_NAME_LENGTH - 1);
        account->user_name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination

        // Parse date_of_creation
        const unsigned char* dateText = sqlite3_column_text(stmt, 3);
        sscanf((const char*)dateText, "%4d-%2d-%2d", &account->creationDate.year, &account->creationDate.month, &account->creationDate.day);

        strncpy(account->country, (const char*)sqlite3_column_text(stmt, 4), MAX_COUNTRY_LENGTH - 1);
        account->country[MAX_COUNTRY_LENGTH - 1] = '\0'; // Ensure null-termination

        strncpy(account->phone, (const char*)sqlite3_column_text(stmt, 5), MAX_PHONE_LENGTH - 1);
        account->phone[MAX_PHONE_LENGTH - 1] = '\0'; // Ensure null-termination

        account->balance = sqlite3_column_double(stmt, 6);

        strncpy(account->type_of_account, (const char*)sqlite3_column_text(stmt, 7), MAX_TYPE_LENGTH - 1);
        account->type_of_account[MAX_TYPE_LENGTH - 1] = '\0'; // Ensure null-termination

        sqlite3_finalize(stmt);
        return 0; // Success
    }

    sqlite3_finalize(stmt);
    return 1; // Account not found
}

// Save a new transaction to the database
int saveTransaction(struct Transaction* transaction) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO transactions (account_id, type, amount, date) VALUES (?, ?, ?, ?);";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in saveTransaction: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, transaction->account_id);
    sqlite3_bind_text(stmt, 2, transaction->type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, transaction->amount);
    sqlite3_bind_text(stmt, 4, transaction->date, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement in saveTransaction: %s\n", sqlite3_errmsg(db));
    } else {
        transaction->id = sqlite3_last_insert_rowid(db);
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Function to view transactions for a specific account
void viewTransactions(int account_id) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT type, amount, date FROM transactions WHERE account_id = ? ORDER BY id DESC;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in viewTransactions: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, account_id);

    int hasTransactions = 0; // Flag to check if any transactions exist

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (!hasTransactions) {
            hasTransactions = 1;
        }
        const char* type = (const char*)sqlite3_column_text(stmt, 0);
        double amount = sqlite3_column_double(stmt, 1);
        const char* date = (const char*)sqlite3_column_text(stmt, 2);
        printf("%-10s\t$%9.2f\t%s\n", type, amount, date);
    }

    if (!hasTransactions) {
        printf("No transactions found for this account.\n");
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement in viewTransactions: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

// Authenticate a user
int authenticateUser(const char* name, const char* password, struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name, password FROM users WHERE name = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in authenticateUser: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char* storedHashedPassword = (const char*)sqlite3_column_text(stmt, 2);
        char inputHashedPassword[SHA256_DIGEST_LENGTH * 2 + 1];
        hashPassword(password, inputHashedPassword);
        if (strcmp(inputHashedPassword, storedHashedPassword) == 0) {
            user->id = sqlite3_column_int(stmt, 0);
            strncpy(user->name, (const char*)sqlite3_column_text(stmt, 1), MAX_NAME_LENGTH - 1);
            user->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
            strncpy(user->password, storedHashedPassword, MAX_PASSWORD_LENGTH - 1);
            user->password[MAX_PASSWORD_LENGTH - 1] = '\0'; // Ensure null-termination
            sqlite3_finalize(stmt);
            return 0; // Authentication successful
        }
    }

    sqlite3_finalize(stmt);
    return 1; // Authentication failed
}

// Add the applyInterest function
int applyInterest(int account_id, const char* current_date) {
    struct Account account;
    if (loadAccount(account_id, &account) != 0) {
        fprintf(stderr, "Account not found.\n");
        return 1;
    }

    double interest_rate = calculateInterestRate(account.type_of_account);
    double interest_amount = 0.0;

    if (strcmp(account.type_of_account, "savings") == 0) {
        // Calculate monthly interest for savings account
        interest_amount = account.balance * interest_rate / 12;
        // Round to 2 decimal places
        interest_amount = round(interest_amount * 100) / 100;
        
        // Debug print
        printf("Debug: Calculated interest before rounding: %f\n", account.balance * interest_rate / 12);
        printf("Debug: Rounded interest: %f\n", interest_amount);

        // Update account balance
        account.balance += interest_amount;
        updateAccount(&account);

        // Save new interest transaction
        struct Transaction interest;
        interest.account_id = account_id;
        strcpy(interest.type, "interest");
        interest.amount = interest_amount;
        strncpy(interest.date, current_date, sizeof(interest.date) - 1);
        interest.date[sizeof(interest.date) - 1] = '\0'; // Ensure null-termination

        return saveTransaction(&interest);
    } else if (strcmp(account.type_of_account, "fixed01") == 0 ||
               strcmp(account.type_of_account, "fixed02") == 0 ||
               strcmp(account.type_of_account, "fixed03") == 0) {
        // Determine the duration in years based on account type
        int durationYears = 0;
        if (strcmp(account.type_of_account, "fixed01") == 0) {
            durationYears = 1;
        } else if (strcmp(account.type_of_account, "fixed02") == 0) {
            durationYears = 2;
        } else if (strcmp(account.type_of_account, "fixed03") == 0) {
            durationYears = 3;
        }

        // Calculate total interest for the duration
        interest_amount = account.balance * interest_rate * durationYears;
    } else if (strcmp(account.type_of_account, "current") == 0) {
        // Current accounts do not earn interest
        printf("You will not earn interest because the account is of type current.\n");
        return 0;
    } else {
        fprintf(stderr, "Unknown account type.\n");
        return 1;
    }

    // Round the interest amount to the nearest cent
    interest_amount = round(interest_amount * 100) / 100;

    struct Transaction interest;
    interest.account_id = account_id;
    strcpy(interest.type, "interest");
    interest.amount = interest_amount;
    strncpy(interest.date, current_date, sizeof(interest.date) - 1);
    interest.date[sizeof(interest.date) - 1] = '\0'; // Ensure null-termination

    return saveTransaction(&interest);
}

// Update an existing account
int updateAccount(struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE accounts SET user_id = ?, user_name = ?, country = ?, phone = ?, balance = ?, type_of_account = ? WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in updateAccount: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, account->user_id);
    sqlite3_bind_text(stmt, 2, account->user_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, account->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, account->phone, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, account->balance);
    sqlite3_bind_text(stmt, 6, account->type_of_account, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, account->id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement in updateAccount: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Delete an account by id
int deleteAccount(int id) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM accounts WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in deleteAccount: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement in deleteAccount: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Update existing interest transaction
int updateInterestTransaction(int account_id, const char* date, double new_amount) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE transactions SET amount = ? WHERE account_id = ? AND type = 'interest' AND date = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in updateInterestTransaction: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_double(stmt, 1, new_amount);
    sqlite3_bind_int(stmt, 2, account_id);
    sqlite3_bind_text(stmt, 3, date, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement in updateInterestTransaction: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}