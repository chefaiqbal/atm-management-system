#include "header.h"

void createNewAccount(struct User* user) {
    if (accountCount >= MAX_ACCOUNTS) {
        printf("Maximum number of accounts reached. Cannot create new accounts.\n");
        return;
    }

    struct Account newAccount;
    newAccount.id = accountCount;
    newAccount.userId = user->id;
    strcpy(newAccount.userName, user->name);
    newAccount.accountId = accountCount + 1000; // Start account IDs from 1000

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    newAccount.creationDate.day = tm.tm_mday;
    newAccount.creationDate.month = tm.tm_mon + 1;
    newAccount.creationDate.year = tm.tm_year + 1900;

    printf("Enter country: ");
    fgets(newAccount.country, sizeof(newAccount.country), stdin);
    newAccount.country[strcspn(newAccount.country, "\n")] = 0;

    printf("Enter phone number: ");
    fgets(newAccount.phone, sizeof(newAccount.phone), stdin);
    newAccount.phone[strcspn(newAccount.phone, "\n")] = 0;

    printf("Enter initial balance: ");
    scanf("%lf", &newAccount.balance);
    getchar(); // Consume newline

    printf("Enter account type (current/savings/fixed01/fixed02/fixed03): ");
    fgets(newAccount.accountType, sizeof(newAccount.accountType), stdin);
    newAccount.accountType[strcspn(newAccount.accountType, "\n")] = 0;

    accounts[accountCount++] = newAccount;
    printf("Account created successfully. Your account ID is: %d\n", newAccount.accountId);
    saveAccounts(); // Save accounts after creating a new one
}

void checkAccountDetails(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to check: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    int accountIndex = findAccount(accountId);
    if (accountIndex == -1 || accounts[accountIndex].userId != user->id) {
        printf("Account not found or you don't have permission to access it.\n");
        return;
    }

    struct Account* account = &accounts[accountIndex];
    printf("\nAccount Details:\n");
    printf("Account ID: %d\n", account->accountId);
    printf("User Name: %s\n", account->userName);
    printf("Creation Date: %02d/%02d/%04d\n", account->creationDate.day, account->creationDate.month, account->creationDate.year);
    printf("Country: %s\n", account->country);
    printf("Phone: %s\n", account->phone);
    printf("Balance: $%.2f\n", account->balance);
    printf("Account Type: %s\n", account->accountType);

    double interest = calculateInterest(account);
    if (interest > 0) {
        printf("You will get $%.2f as interest on day %d of every month\n", interest, account->creationDate.day);
    } else {
        printf("You will not get interests because the account is of type current\n");
    }
}

void updateAccountInfo(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to update: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    int accountIndex = findAccount(accountId);
    if (accountIndex == -1 || accounts[accountIndex].userId != user->id) {
        printf("Account not found or you don't have permission to update it.\n");
        return;
    }

    struct Account* account = &accounts[accountIndex];
    int choice;
    printf("What do you want to update?\n");
    printf("1. Country\n");
    printf("2. Phone number\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline

    switch (choice) {
        case 1:
            printf("Enter new country: ");
            fgets(account->country, sizeof(account->country), stdin);
            account->country[strcspn(account->country, "\n")] = 0;
            printf("Country updated successfully.\n");
            break;
        case 2:
            printf("Enter new phone number: ");
            fgets(account->phone, sizeof(account->phone), stdin);
            account->phone[strcspn(account->phone, "\n")] = 0;
            printf("Phone number updated successfully.\n");
            break;
        default:
            printf("Invalid choice. Update cancelled.\n");
    }
    printf("Account information updated successfully.\n");
    saveAccounts(); // Save accounts after updating
}

void removeAccount(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to remove: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    int accountIndex = findAccount(accountId);
    if (accountIndex == -1 || accounts[accountIndex].userId != user->id) {
        printf("Account not found or you don't have permission to remove it.\n");
        return;
    }

    // Remove the account by shifting all subsequent accounts
    for (int i = accountIndex; i < accountCount - 1; i++) {
        accounts[i] = accounts[i + 1];
    }
    accountCount--;

    printf("Account removed successfully.\n");
    saveAccounts(); // Save accounts after removing one
}

void checkOwnedAccounts(struct User* user) {
    printf("\nYour Accounts:\n");
    printf("ID\tType\t\tBalance\n");
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].userId == user->id) {
            printf("%d\t%-10s\t$%.2f\n", accounts[i].accountId, accounts[i].accountType, accounts[i].balance);
        }
    }
}

void makeTransaction(struct User* user) {
    int accountId;
    printf("Enter the account ID for the transaction: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    int accountIndex = findAccount(accountId);
    if (accountIndex == -1 || accounts[accountIndex].userId != user->id) {
        printf("Account not found or you don't have permission to access it.\n");
        return;
    }

    struct Account* account = &accounts[accountIndex];
    if (strcmp(account->accountType, "fixed01") == 0 ||
        strcmp(account->accountType, "fixed02") == 0 ||
        strcmp(account->accountType, "fixed03") == 0) {
        printf("Transactions are not allowed for fixed deposit accounts.\n");
        return;
    }

    int choice;
    double amount;
    printf("1. Deposit\n");
    printf("2. Withdraw\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline

    printf("Enter the amount: $");
    scanf("%lf", &amount);
    getchar(); // Consume newline

    switch (choice) {
        case 1:
            account->balance += amount;
            printf("Deposit successful. New balance: $%.2f\n", account->balance);
            break;
        case 2:
            if (amount > account->balance) {
                printf("Insufficient funds. Withdrawal cancelled.\n");
            } else {
                account->balance -= amount;
                printf("Withdrawal successful. New balance: $%.2f\n", account->balance);
            }
            break;
        default:
            printf("Invalid choice. Transaction cancelled.\n");
    }
    printf("Transaction completed successfully.\n");
    saveAccounts(); // Save accounts after a transaction
}

void transferOwnership(struct User* user) {
    int accountId;
    printf("Enter the account ID you want to transfer: ");
    scanf("%d", &accountId);
    getchar(); // Consume newline

    int accountIndex = findAccount(accountId);
    if (accountIndex == -1 || accounts[accountIndex].userId != user->id) {
        printf("Account not found or you don't have permission to transfer it.\n");
        return;
    }

    char newOwnerName[MAX_NAME_LENGTH];
    printf("Enter the name of the new owner: ");
    fgets(newOwnerName, sizeof(newOwnerName), stdin);
    newOwnerName[strcspn(newOwnerName, "\n")] = 0;

    int newOwnerIndex = findUser(newOwnerName);
    if (newOwnerIndex == -1) {
        printf("New owner not found. Transfer cancelled.\n");
        return;
    }

    accounts[accountIndex].userId = users[newOwnerIndex].id;
    strcpy(accounts[accountIndex].userName, users[newOwnerIndex].name);
    printf("Ownership transferred successfully.\n");
    saveAccounts(); // Save accounts after transferring ownership
}

double calculateInterest(struct Account* account) {
    double interestRate = 0.0;
    if (strcmp(account->accountType, "savings") == 0) {
        interestRate = 0.07;
    } else if (strcmp(account->accountType, "fixed01") == 0) {
        interestRate = 0.04;
    } else if (strcmp(account->accountType, "fixed02") == 0) {
        interestRate = 0.05;
    } else if (strcmp(account->accountType, "fixed03") == 0) {
        interestRate = 0.08;
    }

    return account->balance * interestRate / 12; // Monthly interest
}