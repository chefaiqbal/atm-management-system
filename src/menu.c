#include "header.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

extern sqlite3* db;

// Display the main menu with options to Login, Register, or Exit
void mainMenu(void) {
    int choice;
    do {
        system("clear");
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
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
                sleep(1);
        }
    } while (1);
}

// Display the user-specific menu after successful login
void userMenu(struct User* user) {
    int choice;
    do {
        system("clear");
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
                break;
            case 2:
                checkAccountDetails(user);
                break;
            case 3:
                updateAccountInfo(user);
                break;
            case 4:
                removeAccount(user);
                break;
            case 5:
                checkOwnedAccounts(user);
                break;
            case 6:
                makeTransaction(user);
                break;
            case 7:
                transferOwnership(user);
                break;
            case 8:
                printf("Logging out. Goodbye, %s!\n", user->name);
                printf("Press Enter to continue...");
                getchar(); // Wait for user to press Enter
                mainMenu(); // Return to main menu
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                sleep(1);
        }
    } while (1);
}