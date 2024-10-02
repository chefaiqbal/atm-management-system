#include "header.h"
#include <sys/stat.h>
#include <errno.h>
#include <sqlite3.h>

struct User users[MAX_USERS];
int userCount = 0;
struct Account accounts[MAX_ACCOUNTS];
int accountCount = 0;
sqlite3* db; // Defined here and used across the application

// Ensure the data directory exists
void ensureDataDirectoryExists(void) {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        #ifdef _WIN32
            _mkdir("data");
        #else
            mkdir("data", 0700);
        #endif
    }
}

// Initialize the SQLite database and create tables if they don't exist
int initDatabase(void) {
    int rc = sqlite3_open("data/bank.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return rc;
    }

    const char *sql_create_users = "CREATE TABLE IF NOT EXISTS users ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT UNIQUE NOT NULL,"
                                   "password TEXT NOT NULL);";

    const char *sql_create_accounts = "CREATE TABLE IF NOT EXISTS accounts ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "userId INTEGER NOT NULL,"
                                      "userName TEXT NOT NULL,"
                                      "accountId INTEGER UNIQUE NOT NULL,"
                                      "creationDate TEXT NOT NULL,"
                                      "country TEXT NOT NULL,"
                                      "phone TEXT NOT NULL,"
                                      "balance REAL NOT NULL,"
                                      "accountType TEXT NOT NULL,"
                                      "FOREIGN KEY (userId) REFERENCES users(id));";

    char *errmsg = NULL;
    int rc_exec = sqlite3_exec(db, sql_create_users, 0, 0, &errmsg);
    if (rc_exec != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return rc_exec;
    }

    rc_exec = sqlite3_exec(db, sql_create_accounts, 0, 0, &errmsg);
    if (rc_exec != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return rc_exec;
    }

    return SQLITE_OK;
}

// Close the SQLite database
int closeDatabase(void) {
    return sqlite3_close(db);
}

// Main function to start the ATM system
int main(void) {
    ensureDataDirectoryExists();
    
    if (initDatabase() != SQLITE_OK) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }

    mainMenu();  // Launch the main menu

    if (closeDatabase() != SQLITE_OK) {
        fprintf(stderr, "Failed to close database\n");
        return 1;
    }

    return 0;
}