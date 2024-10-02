#include "header.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

extern sqlite3* db;

int saveUser(struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO users (id, name, password) VALUES (?, ?, ?);";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, user->id);
    sqlite3_bind_text(stmt, 2, user->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc;
}

int loadUser(const char* name, struct User* user) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name, password FROM users WHERE name = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        strcpy(user->name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(user->password, (const char*)sqlite3_column_text(stmt, 2));
        sqlite3_finalize(stmt);
        return 0;
    } else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return 1; // User not found
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
}

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

int saveAccount(struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO accounts (id, userId, userName, accountId, creationDate, country, phone, balance, accountType) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    char creationDate[11];
    snprintf(creationDate, sizeof(creationDate), "%02d/%02d/%04d", 
             account->creationDate.day, account->creationDate.month, account->creationDate.year);

    sqlite3_bind_int(stmt, 1, account->id);
    sqlite3_bind_int(stmt, 2, account->userId);
    sqlite3_bind_text(stmt, 3, account->userName, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, account->accountId);
    sqlite3_bind_text(stmt, 5, creationDate, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, account->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, account->phone, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 8, account->balance);
    sqlite3_bind_text(stmt, 9, account->accountType, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc;
}

int loadAccount(int accountId, struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT * FROM accounts WHERE accountId = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, accountId);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        account->id = sqlite3_column_int(stmt, 0);
        account->userId = sqlite3_column_int(stmt, 1);
        strcpy(account->userName, (const char*)sqlite3_column_text(stmt, 2));
        account->accountId = sqlite3_column_int(stmt, 3);
        sscanf((const char*)sqlite3_column_text(stmt, 4), "%d/%d/%d", 
               &account->creationDate.day,
               &account->creationDate.month,
               &account->creationDate.year);
        strcpy(account->country, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(account->phone, (const char*)sqlite3_column_text(stmt, 6));
        account->balance = sqlite3_column_double(stmt, 7);
        strcpy(account->accountType, (const char*)sqlite3_column_text(stmt, 8));
        sqlite3_finalize(stmt);
        return 0;
    } else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return 1; // Account not found
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
}

int updateAccount(struct Account* account) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE accounts SET userId = ?, userName = ?, creationDate = ?, country = ?, phone = ?, balance = ?, accountType = ? WHERE accountId = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    char creationDate[11];
    snprintf(creationDate, sizeof(creationDate), "%02d/%02d/%04d", 
             account->creationDate.day, account->creationDate.month, account->creationDate.year);

    sqlite3_bind_int(stmt, 1, account->userId);
    sqlite3_bind_text(stmt, 2, account->userName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, creationDate, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, account->country, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, account->phone, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, account->balance);
    sqlite3_bind_text(stmt, 7, account->accountType, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, account->accountId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc;
}

int deleteAccount(int accountId) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM accounts WHERE accountId = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, accountId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc;
}

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
        const char* storedPassword = (const char*)sqlite3_column_text(stmt, 2);
        if (strcmp(password, storedPassword) == 0) {
            user->id = sqlite3_column_int(stmt, 0);
            strncpy(user->name, (const char*)sqlite3_column_text(stmt, 1), MAX_NAME_LENGTH - 1);
            user->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
            strncpy(user->password, storedPassword, MAX_PASSWORD_LENGTH - 1);
            user->password[MAX_PASSWORD_LENGTH - 1] = '\0'; // Ensure null-termination
            sqlite3_finalize(stmt);
            return 0; // Authentication successful
        }
    }

    sqlite3_finalize(stmt);
    return 1; // Authentication failed
}

// **Optional**: Find a user by name and return user ID
int findUser(const char* name)
{
    struct User user;
    if (loadUser(name, &user) == 0) {
        return user.id;
    }
    return -1;
}