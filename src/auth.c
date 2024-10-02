#define _POSIX_C_SOURCE 200809L
#include "header.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sqlite3.h>

extern char a[50];  // For username
extern char pass[50];

// Add this function prototype
int authenticateUser(const char* name, const char* password, struct User* user);



extern int getch(void);

void loginMenu(void)
{
    struct User user;
    int authenticated = 0;
    int failedAttempts = 0;

    while (failedAttempts < 3)
    {
        system("clear");
        printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:\n");
        printf("\n\t\t\t\tEnter your username: ");
        char username[MAX_NAME_LENGTH];
        scanf("%s", username);

        // Disable echo for password input
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        printf("\n\t\t\t\tEnter your password: ");
        char password[MAX_PASSWORD_LENGTH];
        scanf("%s", password);

        // Restore terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        printf("\n");

        if (authenticateUser(username, password, &user) == 0)
        {
            authenticated = 1;
            break;
        }
        else
        {
            failedAttempts++;
            printf("Invalid username or password! Attempt %d of 3\n", failedAttempts);
            printf("Press Enter to continue...");
            getchar(); // Wait for user to press Enter
        }
    }

    if (authenticated)
    {
        printf("Login successful!\n");
        printf("Press Enter to continue...");
        getchar(); // Wait for user input
        userMenu(&user); // Correct function call
    }
    else
    {
        printf("Too many failed login attempts. Exiting...\n");
        exit(1);
    }
}

void registerUser(void)
{
    struct User newUser;
    char username[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    
    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Registration:\n");
    printf("\n\t\t\t\tEnter your desired username: ");
    scanf("%s", username);
    
    printf("\n\t\t\t\tEnter your desired password: ");
    scanf("%s", password);
    
    // Check if username already exists
    if (loadUser(username, &newUser) == 0)
    {
        printf("\n\t\t\t\tUsername already exists. Please choose a different username.\n");
        printf("Press Enter to continue...");
        getchar(); // Wait for user to press Enter
        getchar();
        return;
    }

    strcpy(newUser.name, username);
    strcpy(newUser.password, password);

    if (saveUser(&newUser) != 0)
    {
        printf("\n\t\t\t\tError registering user. Please try again.\n");
    }
    else
    {
        printf("\n\t\t\t\tRegistration successful! Your user ID is: %d\n", newUser.id);
    }
    printf("Press Enter to continue...");
    getchar(); // Wait for user to press Enter
    getchar();
}