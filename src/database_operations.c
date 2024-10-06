#include "header.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <openssl/sha.h>

// Helper function to hash a password using SHA256
void hashPassword(const char* password, char* hashedPassword) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, strlen(password), hash);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashedPassword + (i * 2), "%02x", hash[i]);
    }
    hashedPassword[SHA256_DIGEST_LENGTH * 2] = '\0';
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
        return 0; // User found
    }

    sqlite3_finalize(stmt);
    return 1; // User not found
}

// Load an account by ID
int loadAccount(int accountId, struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, user_id, user_name, date_of_creation, country, phone, balance, type_of_account FROM accounts WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in loadAccount: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, accountId);

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
        return 0; // Account found
    }

    sqlite3_finalize(stmt);
    return 1; // Account not found
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
    snprintf(creationDateStr, sizeof(creationDateStr), "%04d-%02d-%02d", account->creationDate.year, account->creationDate.month, account->creationDate.day);

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
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Update an existing account
int updateAccount(struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE accounts SET user_id = ?, user_name = ?, type_of_account = ?, balance = ? WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in updateAccount: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, account->user_id);
    sqlite3_bind_text(stmt, 2, account->user_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, account->type_of_account, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, account->balance);
    sqlite3_bind_int(stmt, 5, account->id);

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