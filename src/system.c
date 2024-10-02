#include "header.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <string.h>

// Retrieve the current date
void getCurrentDate(struct Date* date) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    date->day = tm.tm_mday;
    date->month = tm.tm_mon + 1;
    date->year = tm.tm_year + 1900;
}

// Prompt the user to continue or exit after an operation
void success(struct User* u) {
    int choice;
    printf("\n\nDo you want to continue?\n");
    printf("1. Yes\n");
    printf("2. No\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline
    switch (choice) {
        case 1:
            userMenu(u);
            break;
        case 2:
            printf("Thank you for using our ATM. Goodbye!\n");
            exit(0);
        default:
            printf("Invalid choice\n");
            success(u);
    }
}

// Get a single character input without echoing to the terminal
int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Retrieve interest rate based on account type
float getInterestRate(const char* accountType) {
    if (strcmp(accountType, "savings") == 0) return 0.07f;
    if (strcmp(accountType, "fixed01") == 0) return 0.04f;
    if (strcmp(accountType, "fixed02") == 0) return 0.05f;
    if (strcmp(accountType, "fixed03") == 0) return 0.08f;
    return 0.0f; // Current accounts do not receive interest
}