#include "header.h"
#include <limits.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>


char *RECORDS = "./data/records.txt";

void getCurrentDate(struct Date *date) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    date->day = tm.tm_mday;
    date->month = tm.tm_mon + 1; // tm_mon is 0-11, so add 1
    date->year = tm.tm_year + 1900; // tm_year is years since 1900
}

void createNewAcc(struct User u) {
    struct Record r;
    FILE *pf;
    int accountNbr;

    system("clear");
    printf("\t\t====== Create New Account =====\n\n");

    printf("Enter account number: ");
    accountNbr = get_int_input(0, INT_MAX);
    if (accountNbr == -1) {
        mainMenu(u); // Call the main menu function to go back
        return;
    }

    r.accountNbr = accountNbr;
    r.userId = u.id;

    printf("Enter name: ");
    scanf("%s", r.name);

    printf("Enter country: ");
    scanf("%s", r.country);

    printf("Enter phone number: ");
    scanf("%d", &r.phone);

    printf("Enter account type (saving/fixed01/fixed02/fixed03): ");
    scanf("%s", r.accountType);

    printf("Enter initial deposit amount: ");
    scanf("%lf", &r.amount);

    // Automatically set the current date
    getCurrentDate(&r.deposit);

    pf = fopen("data/records.txt", "a");
    if (pf == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Format and save the record to the file
    fprintf(pf, "%d %d %s %s %d %.6lf %s %d/%d/%d %d/%d/%d\n",
            r.id, r.userId, r.name, r.country, r.phone, r.amount, r.accountType,
            r.deposit.day, r.deposit.month, r.deposit.year,
            r.withdraw.day, r.withdraw.month, r.withdraw.year);

    fclose(pf);

    printf("\nAccount created successfully!\n");
    success(u);
}

void checkAllAccounts(struct User u)
{
    char userName[100];
    struct Record r;
    FILE *pf;
    char choice;

    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);

    pf = fopen(RECORDS, "r");
    if (pf == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    printf("Enter 'b' to go back or press Enter to continue: ");
    choice = getchar();
    if (choice == 'b')
    {
        mainMenu(u); // Call the main menu function to go back
        fclose(pf);
        return;
    }

    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0)
        {
            printf("_____________________\n");
            printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \nCountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
                   r.accountNbr,
                   r.deposit.day,
                   r.deposit.month,
                   r.deposit.year,
                   r.country,
                   r.phone,
                   r.amount,
                   r.accountType);
        }
    }

    printf("\nNo more accounts to display.\n");

    fclose(pf);
    success(u);
}

void updateAccountInfo(struct User u)
{
    int accountNbr;
    struct Record r;
    FILE *pf, *temp;
    int found = 0;

    system("clear");
    printf("\t\t====== Update Account Information =====\n\n");
    printf("Enter the account number to update (or 'b' to go back): ");
    accountNbr = get_int_input(0, INT_MAX);
    if (accountNbr == -1) {
        mainMenu(u); // Call the main menu function to go back
        return;
    }

    pf = fopen(RECORDS, "r");
    temp = fopen("temp.txt", "w");

    while (getAccountFromFile(pf, r.name, &r))
    {
        if (r.accountNbr == accountNbr && r.userId == u.id)
        {
            found = 1;
            printf("\nEnter the new country: ");
            scanf("%s", r.country);
            printf("\nEnter the new phone number: ");
            scanf("%d", &r.phone);
        }
        saveAccountToFile(temp, u, r);
    }

    fclose(pf);
    fclose(temp);

    remove(RECORDS);
    rename("temp.txt", RECORDS);

    if (found)
    {
        printf("\nAccount information updated successfully!\n");
    }
    else
    {
        printf("\nAccount not found!\n");
    }
    success(u);
}

void makeTransaction(struct User u) {
    int accountNbr;
    double amount;
    struct Record r;
    FILE *pf, *temp;
    int found = 0;

    system("clear");
    printf("\t\t====== Make Transaction =====\n\n");
    printf("Enter the account number to make a transaction (or 'b' to go back): ");
    accountNbr = get_int_input(0, INT_MAX);
    if (accountNbr == -1) {
        mainMenu(u); // Call the main menu function to go back
        return;
    }

    printf("Enter the amount to deposit/withdraw (negative for withdrawal): ");
    if (scanf("%lf", &amount) != 1) {
        printf("Invalid input. Please enter a valid amount.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }

    pf = fopen("data/records.txt", "r");
    temp = fopen("temp.txt", "w");

    while (getAccountFromFile(pf, r.name, &r)) {
        if (r.accountNbr == accountNbr && r.userId == u.id) {
            found = 1;
            r.amount += amount;
            if (r.amount < 0) {
                printf("Insufficient funds.\n");
                r.amount -= amount; // Revert the transaction
            }
        }
        saveAccountToFile(temp, u, r);
    }

    fclose(pf);
    fclose(temp);

    remove("data/records.txt");
    rename("temp.txt", "data/records.txt");

    if (found) {
        printf("\nTransaction completed successfully!\n");
    } else {
        printf("\nAccount not found!\n");
    }
    success(u);
}

void checkAccounts(struct User u)
{
    int accountNbr;
    struct Record r;
    FILE *pf;
    int found = 0;

    while (1)
    {
        system("clear");
        printf("\t\t====== Check Account =====\n\n");
        printf("Enter the account number to check (or 'b' to go back): ");
        accountNbr = get_int_input(0, INT_MAX);
        if (accountNbr == -1) {
            mainMenu(u); // Call the main menu function to go back
            return;
        }

        pf = fopen(RECORDS, "r");
        if (pf == NULL)
        {
            printf("Error! opening file");
            exit(1);
        }

        found = 0;
        while (getAccountFromFile(pf, r.name, &r))
        {
            if (r.accountNbr == accountNbr && r.userId == u.id)
            {
                found = 1;
                printf("_____________________\n");
                printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \nCountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
                       r.accountNbr, r.deposit.day, r.deposit.month, r.deposit.year, r.country, r.phone, r.amount, r.accountType);
            }
        }

        fclose(pf);

        if (!found)
        {
            printf("\nAccount not found!\n");
        }

        printf("\nPress Enter to continue...");
        while (getchar() != '\n'); // Wait for user to press Enter
        getchar(); // Consume the newline character
    }
}
// Function to remove an account
void removeAccount(struct User u)
{
    int accountNbr;
    struct Record r;
    FILE *pf, *temp;
    int found = 0;

    while (1)
    {
        system("clear");
        printf("\t\t====== Remove Account =====\n\n");
        printf("Enter the account number to remove (or 'b' to go back): ");
        accountNbr = get_int_input(0, INT_MAX);
        if (accountNbr == -1) {
            mainMenu(u); // Call the main menu function to go back
            return;
        }

        pf = fopen(RECORDS, "r");
        temp = fopen("temp.txt", "w");

        while (getAccountFromFile(pf, r.name, &r))
        {
            if (r.accountNbr != accountNbr || r.userId != u.id)
            {
                saveAccountToFile(temp, u, r);
            }
            else
            {
                found = 1;
            }
        }

        fclose(pf);
        fclose(temp);

        remove(RECORDS);
        rename("temp.txt", RECORDS);

        if (found)
        {
            printf("\nAccount removed successfully!\n");
        }
        else
        {
            printf("\nAccount not found!\n");
        }
        success(u);
        return;
    }
}
// Function to transfer ownership of an account
void transferOwnership(struct User u)
{
    int accountNbr, newUserId;
    struct Record r;
    FILE *pf, *temp;
    int found = 0;

    while (1)
    {
        system("clear");
        printf("\t\t====== Transfer Ownership =====\n\n");
        printf("Enter the account number to transfer (or 'b' to go back): ");
        accountNbr = get_int_input(0, INT_MAX);
        if (accountNbr == -1) {
            mainMenu(u); // Call the main menu function to go back
            return;
        }

        printf("Enter the new user ID: ");
        newUserId = get_int_input(0, INT_MAX);
        if (newUserId == -1) {
            mainMenu(u); // Call the main menu function to go back
            return;
        }

        pf = fopen(RECORDS, "r");
        temp = fopen("temp.txt", "w");

        while (getAccountFromFile(pf, r.name, &r))
        {
            if (r.accountNbr == accountNbr && r.userId == u.id)
            {
                found = 1;
                r.userId = newUserId;
            }
            saveAccountToFile(temp, u, r);
        }

        fclose(pf);
        fclose(temp);

        remove(RECORDS);
        rename("temp.txt", RECORDS);

        if (found)
        {
            printf("\nOwnership transferred successfully!\n");
        }
        else
        {
            printf("\nAccount not found!\n");
        }
        success(u);
        return;
    }
}
// Utility function to indicate success
void success(struct User u)
{
    printf("\nOperation completed successfully!\n");
}

// Utility function to get an account from file
int getAccountFromFile(FILE *pf, char *userName, struct Record *r)
{
    return fscanf(pf, "%d %d %s %s %d %lf %s %d/%d/%d %d/%d/%d", &r->id, &r->userId, r->name, r->country, &r->phone, &r->amount, r->accountType, &r->deposit.day, &r->deposit.month, &r->deposit.year, &r->withdraw.day, &r->withdraw.month, &r->withdraw.year) != EOF;
}

// Utility function to save an account to file
void saveAccountToFile(FILE *pf, struct User u, struct Record r) {
    fprintf(pf, "%d %d %s %s %d %.6lf %s %d/%d/%d %d/%d/%d\n",
            r.id, r.userId, r.name, r.country, r.phone, r.amount, r.accountType,
            r.deposit.day, r.deposit.month, r.deposit.year,
            r.withdraw.day, r.withdraw.month, r.withdraw.year);
}

// Utility function to wait for a key press
int getch()
{
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

int accountBelongsToUser(int userId, int accountId) {
    FILE* fp = fopen("../data/records.txt", "r");
    if (fp == NULL) {
        return 0;
    }

    int id, user_id, acc_id;
    char name[50], date[20], country[50], phone[20], type[20];
    float balance;

    while (fscanf(fp, "%d %d %s %d %s %s %s %f %s", &id, &user_id, name, &acc_id, date, country, phone, &balance, type) == 9) {
        if (user_id == userId && acc_id == accountId) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int updateAccountField(int accountId, const char* field, const char* newValue) {
    FILE* fp = fopen("../data/records.txt", "r");
    FILE* tempFp = fopen("../data/temp_records.txt", "w");

    if (fp == NULL || tempFp == NULL) {
        if (fp) fclose(fp);
        if (tempFp) fclose(tempFp);
        return 0;
    }

    int id, user_id, acc_id;
    char name[50], date[20], country[50], phone[20], type[20];
    float balance;
    int updated = 0;

    while (fscanf(fp, "%d %d %s %d %s %s %s %f %s", &id, &user_id, name, &acc_id, date, country, phone, &balance, type) == 9) {
        if (acc_id == accountId) {
            if (strcmp(field, "country") == 0) {
                strcpy(country, newValue);
            } else if (strcmp(field, "phone") == 0) {
                strcpy(phone, newValue);
            }
            updated = 1;
        }
        fprintf(tempFp, "%d %d %s %d %s %s %s %.2f %s\n", id, user_id, name, acc_id, date, country, phone, balance, type);
    }

    fclose(fp);
    fclose(tempFp);

    if (updated) {
        remove("../data/records.txt");
        rename("../data/temp_records.txt", "../data/records.txt");
        return 1;
    } else {
        remove("../data/temp_records.txt");
        return 0;
    }
}

void displayAccountDetails(int accountId) {
    FILE* fp = fopen("../data/records.txt", "r");
    if (fp == NULL) {
        printf("\n\tError: Unable to open records file.\n");
        return;
    }

    int id, user_id, acc_id;
    char name[50], date[20], country[50], phone[20], type[20];
    float balance;
    int found = 0;

    while (fscanf(fp, "%d %d %s %d %s %s %s %f %s", &id, &user_id, name, &acc_id, date, country, phone, &balance, type) == 9) {
        if (acc_id == accountId) {
            found = 1;
            printf("\n\tAccount Details:");
            printf("\n\t------------------");
            printf("\n\tAccount ID: %d", acc_id);
            printf("\n\tName: %s", name);
            printf("\n\tDate of Creation: %s", date);
            printf("\n\tCountry: %s", country);
            printf("\n\tPhone: %s", phone);
            printf("\n\tBalance: $%.2f", balance);
            printf("\n\tAccount Type: %s", type);

            if (strcmp(type, "current") == 0) {
                printf("\n\n\tYou will not get interests because the account is of type current.");
            } else {
                float interestRate = getInterestRate(type);
                float monthlyInterest = (balance * interestRate) / 12;
                printf("\n\n\tYou will get $%.2f as interest on day %s of every month.", monthlyInterest, date);
            }
            break;
        }
    }

    fclose(fp);

    if (!found) {
        printf("\n\tError: Account not found.\n");
    }
}

float getInterestRate(const char* accountType) {
    if (strcmp(accountType, "savings") == 0) return 0.07;
    if (strcmp(accountType, "fixed01") == 0) return 0.04;
    if (strcmp(accountType, "fixed02") == 0) return 0.05;
    if (strcmp(accountType, "fixed03") == 0) return 0.08;
    return 0.0; // Default case (shouldn't happen)
}

int performTransaction(int accountId, int transactionType, float amount) {
    FILE* fp = fopen("../data/records.txt", "r");
    FILE* tempFp = fopen("../data/temp_records.txt", "w");

    if (fp == NULL || tempFp == NULL) {
        if (fp) fclose(fp);
        if (tempFp) fclose(tempFp);
        return -2; // Error opening files
    }

    int id, user_id, acc_id;
    char name[50], date[20], country[50], phone[20], type[20];
    float balance;
    int updated = 0;

    while (fscanf(fp, "%d %d %s %d %s %s %s %f %s", &id, &user_id, name, &acc_id, date, country, phone, &balance, type) == 9) {
        if (acc_id == accountId) {
            if (strncmp(type, "fixed", 5) == 0) {
                fprintf(tempFp, "%d %d %s %d %s %s %s %.2f %s\n", id, user_id, name, acc_id, date, country, phone, balance, type);
                fclose(fp);
                fclose(tempFp);
                remove("../data/temp_records.txt");
                return -1; // Transaction not allowed for fixed accounts
            }

            if (transactionType == 1) { // Deposit
                balance += amount;
                updated = 1;
            } else if (transactionType == 2) { // Withdraw
                if (balance >= amount) {
                    balance -= amount;
                    updated = 1;
                } else {
                    fprintf(tempFp, "%d %d %s %d %s %s %s %.2f %s\n", id, user_id, name, acc_id, date, country, phone, balance, type);
                    fclose(fp);
                    fclose(tempFp);
                    remove("../data/temp_records.txt");
                    return 0; // Insufficient balance
                }
            }
        }
        fprintf(tempFp, "%d %d %s %d %s %s %s %.2f %s\n", id, user_id, name, acc_id, date, country, phone, balance, type);
    }

    fclose(fp);
    fclose(tempFp);

    if (updated) {
        remove("../data/records.txt");
        rename("../data/temp_records.txt", "../data/records.txt");
        return 1; // Transaction successful
    } else {
        remove("../data/temp_records.txt");
        return -2; // Error processing transaction
    }
}

int getUserId(const char* username) {
    FILE* fp = fopen("../data/users.txt", "r");
    if (fp == NULL) {
        return -1;
    }

    int id;
    char name[50], password[50];

    while (fscanf(fp, "%d %s %s", &id, name, password) == 3) {
        if (strcmp(name, username) == 0) {
            fclose(fp);
            return id;
        }
    }

    fclose(fp);
    return -1; // User not found
}

int changeAccountOwner(int accountId, int newOwnerId) {
    FILE* fp = fopen("../data/records.txt", "r");
    FILE* tempFp = fopen("../data/temp_records.txt", "w");

    if (fp == NULL || tempFp == NULL) {
        if (fp) fclose(fp);
        if (tempFp) fclose(tempFp);
        return 0;
    }

    int id, user_id, acc_id;
    char name[50], date[20], country[50], phone[20], type[20];
    float balance;
    int updated = 0;

    while (fscanf(fp, "%d %d %s %d %s %s %s %f %s", &id, &user_id, name, &acc_id, date, country, phone, &balance, type) == 9) {
        if (acc_id == accountId) {
            user_id = newOwnerId;
            updated = 1;
        }
        fprintf(tempFp, "%d %d %s %d %s %s %s %.2f %s\n", id, user_id, name, acc_id, date, country, phone, balance, type);
    }

    fclose(fp);
    fclose(tempFp);

    if (updated) {
        remove("../data/records.txt");
        rename("../data/temp_records.txt", "../data/records.txt");
        return 1;
    } else {
        remove("../data/temp_records.txt");
        return 0;
    }
}