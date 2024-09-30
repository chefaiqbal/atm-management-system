#include <termios.h>
#include "header.h"
#include <unistd.h>

char *USERS = "./data/users.txt";

void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;
    struct User userChecker;
    FILE *fp;
    int authenticated = 0;
    int failedAttempts = 0;

    while (failedAttempts < 3)
    {
        system("clear");
        printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
        printf("\n\n\n\t\t\t\tEnter your username: ");
        scanf("%s", a);

        // disabling echo
        tcgetattr(fileno(stdin), &oflags);
        nflags = oflags;
        nflags.c_lflag &= ~ECHO;
        nflags.c_lflag |= ECHONL;

        if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
        {
            perror("tcsetattr");
            exit(1);
        }
        printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
        scanf("%s", pass);

        // restore terminal
        if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
        {
            perror("tcsetattr");
            exit(1);
        }

        // Open the users file to read and authenticate
        if ((fp = fopen(USERS, "r")) == NULL)
        {
            printf("Error! opening file");
            exit(1);
        }

        while (fscanf(fp, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
        {
            if (strcmp(userChecker.name, a) == 0 && strcmp(userChecker.password, pass) == 0)
            {
                authenticated = 1;
                break;
            }
        }

        fclose(fp);

        if (authenticated)
        {
            printf("Login successful!\n");
            getch(); // Wait for user input
            // Proceed to main menu or other functionality
            mainMenu(userChecker);
            return;
        }
        else
        {
            failedAttempts++;
            printf("Invalid username or password! Attempt %d of 3\n", failedAttempts);
            getch(); // Wait for user input
        }
    }

    printf("Too many failed login attempts. Exiting...\n");
    exit(1);
}

// Function to handle user registration
void registerMenu(char a[50], char pass[50])
{
    FILE *fp;
    struct User newUser;
    int id = 0;

    // Check if the username already exists
    FILE *fp_check;
    struct User userChecker;
    if ((fp_check = fopen(USERS, "r")) != NULL)
    {
        while (fscanf(fp_check, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
        {
            if (strcmp(userChecker.name, a) == 0)
            {
                printf("\n\n\n\t\t\t\tUsername already exists. Please choose a different username.");
                fclose(fp_check);
                return;
            }
        }
        fclose(fp_check);
    }

    fp = fopen(USERS, "a+");

    if (fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    // ... (rest of the function remains unchanged)
}

const char *getPassword(struct User u)
{
    FILE *fp;
    struct User userChecker;
    static char password[50]; // Use a static variable to avoid returning the address of a local variable

    if ((fp = fopen("./data/users.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    while (fscanf(fp, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
    {
        if (strcmp(userChecker.name, u.name) == 0)
        {
            fclose(fp);
            strcpy(password, userChecker.password); // Copy the password to the static variable
            return password;
        }
    }

    fclose(fp);
    return "no user found";
}

int userExists(const char* name) {
    FILE* fp = fopen("../data/users.txt", "r");
    if (fp == NULL) {
        return 0;
    }

    int id;
    char storedName[50], password[50];

    while (fscanf(fp, "%d %s %s", &id, storedName, password) == 3) {
        if (strcmp(name, storedName) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int getNextUserId() {
    FILE* fp = fopen("../data/users.txt", "r");
    if (fp == NULL) {
        return 0;
    }

    int maxId = -1;
    int id;
    char name[50], password[50];

    while (fscanf(fp, "%d %s %s", &id, name, password) == 3) {
        if (id > maxId) {
            maxId = id;
        }
    }

    fclose(fp);
    return maxId + 1;
}

int saveUser(int id, const char* name, const char* password) {
    FILE* fp = fopen("../data/users.txt", "a");
    if (fp == NULL) {
        return 0;
    }

    fprintf(fp, "%d %s %s\n", id, name, password);
    fclose(fp);
    return 1;
}