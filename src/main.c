#include "header.h"
#include <sys/stat.h>
#include <errno.h>

struct User users[MAX_USERS];
int userCount = 0;
struct Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

// Add this function at the beginning of main.c
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

int main(void) {
    ensureDataDirectoryExists(); // Call this at the start of main
    loadUsers();
    loadAccounts();
    mainMenu();
    saveUsers();
    saveAccounts();
    return 0;
}

void mainMenu(void) {
    int choice;
    do {
        printf("\n===== ATM System =====\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                loginMenu();
                break;
            case 2:
                registerUser();
                break;
            case 3:
                printf("Thank you for using our ATM. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);
}

void loginMenu(void) {
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // Remove newline

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline

    int userIndex = findUser(name);
    if (userIndex != -1 && strcmp(users[userIndex].password, password) == 0) {
        userMenu(&users[userIndex]);
    } else {
        printf("Invalid username or password.\n");
    }
}

void registerUser(void) {
    if (userCount >= MAX_USERS) {
        printf("Maximum number of users reached. Cannot register new users.\n");
        return;
    }

    struct User newUser;
    char input[MAX_NAME_LENGTH];

    while (1) {
        printf("Enter your name (or 'cancel' to go back): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "cancel") == 0) {
            printf("Registration cancelled.\n");
            return;
        }

        if (findUser(input) != -1) {
            printf("This username already exists. Please choose a different name.\n");
            continue;
        }

        strcpy(newUser.name, input);
        break;
    }

    printf("Enter your password: ");
    fgets(newUser.password, sizeof(newUser.password), stdin);
    newUser.password[strcspn(newUser.password, "\n")] = 0; // Remove newline

    newUser.id = userCount + 1;
    users[userCount++] = newUser;
    printf("Registration successful. Your user ID is: %d\n", newUser.id);
    
    saveUsers(); // Save users after successful registration
}

void userMenu(struct User* user) {
    int choice;
    do {
        printf("\n===== User Menu =====\n");
        printf("1. Create new account\n");
        printf("2. Check account details\n");
        printf("3. Update account information\n");
        printf("4. Remove account\n");
        printf("5. Check owned accounts\n");
        printf("6. Make transaction\n");
        printf("7. Transfer ownership\n");
        printf("8. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                createNewAccount(user);
                saveAccounts(); // Save after creating a new account
                break;
            case 2:
                checkAccountDetails(user);
                break;
            case 3:
                updateAccountInfo(user);
                saveAccounts(); // Save after updating account info
                break;
            case 4:
                removeAccount(user);
                saveAccounts(); // Save after removing an account
                break;
            case 5:
                checkOwnedAccounts(user);
                break;
            case 6:
                makeTransaction(user);
                saveAccounts(); // Save after making a transaction
                break;
            case 7:
                transferOwnership(user);
                saveAccounts(); // Save after transferring ownership
                break;
            case 8:
                printf("Logging out. Goodbye, %s!\n", user->name);
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);
}

void saveUsers(void) {
    FILE* file = fopen("data/users.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing users.\n");
        return;
    }
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%d %s %s\n", users[i].id, users[i].name, users[i].password);
    }
    fclose(file);
    printf("User data saved successfully.\n"); // Add this line for debugging
}

void loadUsers(void) {
    FILE* file = fopen("data/users.txt", "r");
    if (file == NULL) {
        printf("No existing user data found. Starting with empty user list.\n");
        return;
    }
    while (fscanf(file, "%d %s %s", &users[userCount].id, users[userCount].name, users[userCount].password) == 3) {
        userCount++;
    }
    fclose(file);
}

void saveAccounts(void) {
    FILE* file = fopen("data/records.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing accounts.\n");
        return;
    }
    for (int i = 0; i < accountCount; i++) {
        fprintf(file, "%d %d %s %d %02d/%02d/%04d %s %s %.2f %s\n",
                accounts[i].id, accounts[i].userId, accounts[i].userName, accounts[i].accountId,
                accounts[i].creationDate.day, accounts[i].creationDate.month, accounts[i].creationDate.year,
                accounts[i].country, accounts[i].phone, accounts[i].balance, accounts[i].accountType);
    }
    fclose(file);
    printf("Account data saved successfully.\n"); // Add this line for debugging
}

void loadAccounts(void) {
    FILE* file = fopen("data/records.txt", "r");
    if (file == NULL) {
        printf("No existing account data found. Starting with empty account list.\n");
        return;
    }
    while (fscanf(file, "%d %d %s %d %d/%d/%d %s %s %lf %s",
                  &accounts[accountCount].id, &accounts[accountCount].userId, accounts[accountCount].userName,
                  &accounts[accountCount].accountId, &accounts[accountCount].creationDate.day,
                  &accounts[accountCount].creationDate.month, &accounts[accountCount].creationDate.year,
                  accounts[accountCount].country, accounts[accountCount].phone,
                  &accounts[accountCount].balance, accounts[accountCount].accountType) == 11) {
        accountCount++;
    }
    fclose(file);
}

int findUser(const char* name) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int findAccount(int accountId) {
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountId == accountId) {
            return i;
        }
    }
    return -1;
}

// Implement the remaining functions (deposit, withdraw, checkBalance, transferFunds, changePassword)
// in a new file called atm_operations.c