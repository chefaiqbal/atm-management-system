#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_int_input(int min_value, int max_value) {
    int input;
    char choice;
    while (1) {
        if (scanf("%d", &input) != 1) {
            scanf("%c", &choice);
            if (choice == 'b') {
                return -1; // Indicate that the user chose to go back
            }
            printf("Invalid input. Please enter a number between %d and %d: ", min_value, max_value);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (input < min_value || input > max_value) {
            printf("Invalid input. Please enter a number between %d and %d: ", min_value, max_value);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        return input;
    }
}

void initMenu(struct User *u)
{
    int option;
    char username[50], password[50];
    char input[10];
    int authenticated = 0;

    do
    {
        system("clear");
        printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t Main Menu:\n");
        printf("\n\t\t\t\t1. Login");
        printf("\n\t\t\t\t2. Register");
        printf("\n\t\t\t\t3. Exit");
        printf("\n\n\t\t\t\tEnter your choice: ");
        
        // Read input as a string
        fgets(input, sizeof(input), stdin);
        
        // Check if the input is a valid integer
        if (sscanf(input, "%d", &option) != 1) {
            printf("Invalid option! Please try again.\n");
            printf("Press ↵ key to continue...");
            getch(); // Wait for user input
            continue; // Skip the rest of the loop and prompt again
        }
        
        switch (option)
        {
        case 1:
            loginMenu(username, password);
            return;

        case 2:
            registerMenu(username, password);
            break;
        case 3:
            exit(0);

        default:
            printf("Invalid option! Please try again.\n");
            printf("Press ↵ key to continue...");
            getchar(); // Wait for user input
            break;
        }
    } while (1);
}
void mainMenu(struct User u)
{
    int option;
    do
    {
        system("clear");
        printf("\n\n\t\t======= ATM =======\n\n");
        printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
        printf("\n\t\t[1]- Create a new account\n");
        printf("\n\t\t[2]- Update account information\n");
        printf("\n\t\t[3]- Check accounts\n");
        printf("\n\t\t[4]- Check list of owned account\n");
        printf("\n\t\t[5]- Make Transaction\n");
        printf("\n\t\t[6]- Remove existing account\n");
        printf("\n\t\t[7]- Transfer ownership\n");
        printf("\n\t\t[8]- Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            createNewAcc(u);
            break;
        case 2:
            updateAccountInfo(u);
            break;
        case 3:
            checkAccounts(u);
            break;
        case 4:
            checkAllAccounts(u);
            break;
        case 5:
            makeTransaction(u);
            break;
        case 6:
            removeAccount(u);
            break;
        case 7:
            transferOwnership(u);
            break;
        case 8:
            exit(1);
            break;
        default:
            printf("Invalid option! Please try again.\n");
            printf("Press ↵ key to continue...");
        }
            printf("Invalid option! Please try again.\n");
            printf("Press ↵ key to continue...");
        getch(); // Wait for user input
    } while (option != 8);
}

int main()
{
    struct User u;
    
    initMenu(&u);
    mainMenu(u);

    return 0;
}
