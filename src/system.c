#include "header.h"

const char *RECORDS = "./data/records.txt";
const char *USERS = "./data/users.txt";


int getAccountFromFile(FILE *ptr, char name[50], struct Record *r)
{
    return fscanf(ptr, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r->id,
		  &r->userId,
		  name,
                  &r->accountNbr,
                  &r->deposit.month,
                  &r->deposit.day,
                  &r->deposit.year,
                  r->country,
                  &r->phone,
                  &r->amount,
                  r->accountType) != EOF;
}
void saveAccountToFile(FILE *ptr, struct User u, struct Record r)
{
    fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
            r.id,
            u.id,
            u.name,
            r.accountNbr,
            r.deposit.month,
            r.deposit.day,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
}

/*void saveAccountToFile(FILE *ptr, struct User u, struct Record r)
{
    fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
            &r->id,
	    &u->id
	    &u->name,
            r.accountNbr,
            r.deposit.month,
            r.deposit.day,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
}
*/
void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        exit(1);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void createNewAcc(struct User u)
{
    struct Record r;
    struct Record cr;
    char userName[50];
    FILE *pf = fopen(RECORDS, "a+");

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date(mm/dd/yyyy):");
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);

    while (getAccountFromFile(pf, userName, &cr))
    {
        if (strcmp(userName, u.name) == 0 && cr.accountNbr == r.accountNbr)
        {
            printf("✖ This Account already exists for this user\n\n");
            goto noAccount;
        }
    }
    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

    saveAccountToFile(pf, u, r);

    fclose(pf);
    success(u);
}

void checkAllAccounts(struct User u)
{
    char userName[100];
    struct Record r;

    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0)
        {
            printf("_____________________\n");
            printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
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
    fclose(pf);
    success(u);
}
void updateAccountInfo(struct User u) {
    int accountId;
    char fieldToUpdate[20];

    FILE *pf = fopen(RECORDS, "r+");
    struct Record r;

    printf("Enter the account id you want to update: ");
    scanf("%d", &accountId);

    // Check if the account belongs to the current user
    int accountFound = 0;
    while (getAccountFromFile(pf, u.name, &r)) {
        if (r.userId == u.id && r.id == accountId) {
            accountFound = 1;
            break;
        }
    }

    if (!accountFound) {
        printf("Account not found!\n");
        fclose(pf);
        stayOrReturn(0, updateAccountInfo, u);
    }

    printf("Choose the field to update (country/phone): ");
    scanf("%s", fieldToUpdate);

    if (strcmp(fieldToUpdate, "country") == 0) {
        printf("Enter the new country: ");
        scanf("%s", r.country);
    } else if (strcmp(fieldToUpdate, "phone") == 0) {
        printf("Enter the new phone number: ");
        scanf("%d", &r.phone);
    } else {
        printf("Invalid field!\n");
        fclose(pf);
        stayOrReturn(0, updateAccountInfo, u);
    }

    // Update the record in the file
    fseek(pf, -sizeof(struct Record), SEEK_CUR);
    fprintf(pf, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n",
            r.id, r.userId, r.name, r.accountNbr, r.deposit.month, r.deposit.day, r.deposit.year,
            r.country, r.phone, r.amount, r.accountType);

    fclose(pf);
    success(u);
}

void makeTransaction(struct User u) {
    int accountId;
    double amount;
    char transactionType[10];

    FILE *pf = fopen(RECORDS, "r+");
    struct Record r;

    printf("Enter the account id for the transaction: ");
    scanf("%d", &accountId);

    // Check if the account belongs to the current user
    int accountFound = 0;
    while (getAccountFromFile(pf, u.name, &r)) {
        if (r.userId == u.id && r.id == accountId) {
            accountFound = 1;
            break;
        }
    }

    if (!accountFound) {
        printf("Account not found!\n");
        fclose(pf);
        stayOrReturn(0, makeTransaction, u);
    }

    printf("Enter the transaction type (withdrawal/deposit): ");
    scanf("%s", transactionType);

    if (strcmp(transactionType, "withdrawal") == 0) {
        printf("Enter the withdrawal amount: ");
        scanf("%lf", &amount);

        if (strcmp(r.accountType, "fixed01") == 0 || strcmp(r.accountType, "fixed02") == 0 || strcmp(r.accountType, "fixed03") == 0) {
            printf("Withdrawals are not allowed for fixed-term accounts!\n");
            fclose(pf);
            stayOrReturn(0, makeTransaction, u);
        }

        if (amount > r.amount) {
            printf("Insufficient funds!\n");
            fclose(pf);
            stayOrReturn(0, makeTransaction, u);
        }

        r.amount -= amount;
    } else if (strcmp(transactionType, "deposit") == 0) {
        printf("Enter the deposit amount: ");
        scanf("%lf", &amount);

        r.amount += amount;
    } else {
        printf("Invalid transaction type!\n");
        fclose(pf);
        stayOrReturn(0, makeTransaction, u);
    }

    // Update the record in the file
    fseek(pf, -sizeof(struct Record), SEEK_CUR);
    fprintf(pf, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n",
            r.id, r.userId, r.name, r.accountNbr, r.deposit.month, r.deposit.day, r.deposit.year,
            r.country, r.phone, r.amount, r.accountType);

    fclose(pf);
    success(u);
}

void removeExistingAccount(struct User u) {
    int accountId;

    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("./data/temp_records.txt", "w");

    struct Record r;

    printf("Enter the account id you want to remove: ");
    scanf("%d", &accountId);

    int accountFound = 0;
    while (getAccountFromFile(pf, u.name, &r)) {
        if (r.userId == u.id && r.id == accountId) {
            accountFound = 1;
            continue; // Skip writing this record to the temporary file
        }
        fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n",
                r.id, r.userId, r.name, r.accountNbr, r.deposit.month, r.deposit.day, r.deposit.year,
                r.country, r.phone, r.amount, r.accountType);
    }

    fclose(pf);
    fclose(tempFile);

    // Replace the original file with the temporary file
    remove(RECORDS);
    rename("./data/temp_records.txt", RECORDS);

    if (!accountFound) {
        printf("Account not found!\n");
        stayOrReturn(0, removeExistingAccount, u);
    }

    printf("Account successfully removed!\n");
    success(u);
}

void transferOwnership(struct User u) {
    int accountId;
    char newOwnerName[50];

    FILE *pf = fopen(RECORDS, "r+");
    struct Record r;

    printf("Enter the account id you want to transfer: ");
    scanf("%d", &accountId);

    // Check if the account belongs to the current user
    int accountFound = 0;
    while (getAccountFromFile(pf, u.name, &r)) {
        if (r.userId == u.id && r.id == accountId) {
            accountFound = 1;
            break;
        }
    }

    if (!accountFound) {
        printf("Account not found!\n");
        fclose(pf);
        stayOrReturn(0, transferOwnership, u);
    }

    printf("Enter the username of the new owner: ");
    scanf("%s", newOwnerName);

    // Check if the new owner exists
    FILE *usersFile = fopen(USERS, "r");
    struct User newOwner;

    int newOwnerFound = 0;
    while (fscanf(usersFile, "%d %s %s", &newOwner.id, newOwner.name, newOwner.password) != EOF) {
        if (strcmp(newOwner.name, newOwnerName) == 0) {
            newOwnerFound = 1;
            break;
        }
    }

    fclose(usersFile);

    if (!newOwnerFound) {
        printf("New owner not found!\n");
        fclose(pf);
        stayOrReturn(0, transferOwnership, u);
    }

    // Update the record in the file with the new owner's information
    fseek(pf, -sizeof(struct Record), SEEK_CUR);
    fprintf(pf, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n",
            r.id, newOwner.id, newOwner.name, r.accountNbr, r.deposit.month, r.deposit.day, r.deposit.year,
            r.country, r.phone, r.amount, r.accountType);

    fclose(pf);
    success(u);
}
