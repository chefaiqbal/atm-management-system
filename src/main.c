#include "header.h"

void mainMenu(struct User u)
{
    int option;
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
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        createNewAcc(u);
        break;
    case 2:
        // student TODO : add your **Update account information** function
        // here
        break;
    case 3:
        // student TODO : add your **Check the details of existing accounts** function
        // here
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        // student TODO : add your **Make transaction** function
        // here
        break;
    case 6:
        // student TODO : add your **Remove existing account** function
        // here
        break;
    case 7:
        // student TODO : add your **Transfer owner** function
        // here
        break;
    case 8:
        exit(1);
        break;
    default:
        printf("Invalid operation!\n");
    }
};
/*void transferOwnership(struct User u) {
    FILE *recordsFile = fopen(recordsFile, "r");
    FILE *tempFile = fopen("./data/temp_records.txt", "w");

    int accountNbr, targetUserId;
    struct Record record;

    // Get account number to transfer
    printf("Enter the account number to transfer: ");
    scanf("%d", &accountNbr);

    int found = 0;

    // Iterate through records to find the specified account
    while (fscanf(recordsFile, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &record.id,
                  &record.userId,
                  record.name,
                  &record.accountNbr,
                  &record.deposit.month,
                  &record.deposit.day,
                  &record.deposit.year,
                  record.country,
                  &record.phone,
                  &record.amount,
                  record.accountType) != EOF) {
        if (record.accountNbr == accountNbr && strcmp(record.name, u.name) == 0) {
            found = 1;

            // Get target user ID
            printf("Enter the ID of the user to transfer ownership to: ");
            scanf("%d", &targetUserId);

            // Update the user ID in the record
            record.userId = targetUserId;

            printf("Transfer ownership successful!\n");
        }

        // Write the record to the temporary file
        fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n",
                record.id,
                record.userId,
                record.name,
                record.accountNbr,
                record.deposit.month,
                record.deposit.day,
                record.deposit.year,
                record.country,
                record.phone,
                record.amount,
                record.accountType);
    }

    fclose(recordsFile);
    fclose(tempFile);

    // Replace the original records file with the temporary file
    remove(recordsFile);
    rename("./data/temp_records.txt", recordsFile);

    if (!found) {
        printf("Account not found or you don't have permission to transfer ownership.\n");
    }
}
*/
void initMenu(struct User *u)
{
    int r = 0;
    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Feel free to login / register :\n");
    printf("\n\t\t[1]- login\n");
    printf("\n\t\t[2]- register\n");
    printf("\n\t\t[3]- exit\n");
    while (!r)
    {
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            loginMenu(u->name, u->password);
            if (strcmp(u->password, getPassword(*u)) == 0)
            {
                printf("\n\nPassword Match!");
            }
            else
            {
                printf("\nWrong password!! or User Name\n");
                exit(1);
            }
            r = 1;
            break;
        case 2:
            // student TODO : add your **Registration** function
            // here
            r = 1;
            break;
        case 3:
            exit(1);
            break;
        default:
            printf("Insert a valid operation!\n");
        }
    }
};

int main()
{
    struct User u;
    
    initMenu(&u);
    mainMenu(u);
    return 0;
}
