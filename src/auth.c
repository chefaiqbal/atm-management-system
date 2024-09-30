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

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Registration:");
    printf("\n\n\n\t\t\t\tEnter your username: ");
    scanf("%s", a);
    strcpy(newUser.name, a);

    // Disabling echo for password input
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }

    printf("\n\n\n\n\n\t\t\t\tEnter your password: ");
    scanf("%s", pass);
    strcpy(newUser.password, pass);

    // Restoring terminal settings
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }

    // Open the users file to append the new user
    if ((fp = fopen(USERS, "a")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    // Get the next user ID
    FILE *fp_read;
    struct User userChecker;
    if ((fp_read = fopen(USERS, "r")) != NULL)
    {
        while (fscanf(fp_read, "%d %s %s", &id, userChecker.name, userChecker.password) != EOF)
        {
            // Increment the ID for the next user
            id++;
        }
        fclose(fp_read);
    }

    newUser.id = id;

    // Write the new user to the file
    fprintf(fp, "%d %s %s\n", newUser.id, newUser.name, newUser.password);
    fclose(fp);

    printf("\n\n\n\t\t\t\tRegistration successful! Please login to continue.");
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