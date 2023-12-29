#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char debtor[64];
    char creditor[64];
    int amount;
} Transaction;

typedef struct {
    char name[64];
    int balance;
} Person;

int people = 0; 

Person *person;


int minimum(int a, int b) {
    return a < b ? a : b;
}

int isPresent(char *name) { 
    for(int i = 0; i < people; i++) {
        if(strcmp(person[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}


void initializePerson(Transaction* transactions, int num_transactions) {
    // Initialize balances for each person
    int index;

    for (int i = 0; i < num_transactions; ++i) {
        // Extract debtor and creditor information
        char *debtor = transactions[i].debtor;
        char *creditor = transactions[i].creditor;

        // Check if the debtor and creditor are present in the list of people
        if (!isPresent(debtor)) {
            people++;
            person = (Person *)realloc(person, people * sizeof(Person));
            index = people - 1;
            strcpy(person[index].name, debtor);
            person[index].balance = 0;
        }
        if (!isPresent(creditor)) {
            people++;
            person = (Person *)realloc(person, people * sizeof(Person));
            index = people - 1;
            strcpy(person[index].name, creditor);
            person[index].balance = 0;
        }
    }
}

void increaseBalance(char *name, int amount) {
    for(int i = 0; i < people; i++) {
        if(strcmp(person[i].name, name) == 0) {
            person[i].balance += amount;
            break;
        }
    }
}   

void decreaseBalance(char *name, int amount) {
    for(int i = 0; i < people; i++) {
        if(strcmp(person[i].name, name) == 0) {
            person[i].balance -= amount;
            break;
        }
    }
}

int compare(const void *a, const void *b) {
    return ((Person *)a)->balance - ((Person *)b)->balance;
}


void min_transactions(Transaction *transactions, int num_transactions) {

    // Initialize balances for each person    
    initializePerson(transactions, num_transactions);

    for(int i = 0; i < num_transactions; i++) {
        increaseBalance(transactions[i].creditor, transactions[i].amount);
        decreaseBalance(transactions[i].debtor, transactions[i].amount);
    }

    // Sort person array by balance using quicksort
    // TODO: Implement quicksort
    qsort(person, people, sizeof(Person), compare);

    // Initialize pointers for left and right indices
    int i = 0;
    int j = people - 1;

    // Array to store the result transactions
    Transaction *transactions_list = (Transaction *)malloc(0 * sizeof(Transaction)); 

    int num_result_transactions = 0;

    while (i < j) {
        // Extract debtor and creditor information
        char *debtor = person[i].name;
        char *creditor = person[j].name;
        int debtor_balance = person[i].balance;
        int creditor_balance = person[j].balance;

        // Check for zero balances and settle debts
        if (debtor_balance == 0) {
            i++;
        } else if (creditor_balance == 0) {
            j--;
        } else {
            // Determine the minimum transfer amount
            int min_transfer = minimum(-debtor_balance, creditor_balance);

            // Update balances and record the transaction
            debtor_balance += min_transfer;
            creditor_balance -= min_transfer;

            transactions_list = (Transaction *)realloc(transactions_list, (num_result_transactions + 1) * sizeof(Transaction));

            if(min_transfer > 0) {
                transactions_list[num_result_transactions].amount = min_transfer;
                strcpy(transactions_list[num_result_transactions].debtor, debtor);
                strcpy(transactions_list[num_result_transactions].creditor, creditor);
            } else {
                transactions_list[num_result_transactions].amount = -min_transfer;
                strcpy(transactions_list[num_result_transactions].debtor, creditor);
                strcpy(transactions_list[num_result_transactions].creditor, debtor);
            }

            num_result_transactions++;

        }

        // Increment i or decrement j only when the balances are settled
        if (debtor_balance == 0) {
            i++;
        }
        if (creditor_balance == 0) {
            j--;
        }
    }
    
    printf("Optimized Number of transactions for settlemnt of debt: %d\n", num_result_transactions);
    for (int k = 0; k < num_result_transactions; ++k) {
        printf("%s owes %s %d\n", transactions_list[k].debtor, transactions_list[k].creditor, transactions_list[k].amount);
    }


    // Free allocated memory
    free(person);
    free(transactions_list);
}

int number_of_lines_in_a_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Count the number of lines in the file
    int num_lines = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            num_lines++;
        }
    }

    // Close the file
    fclose(fp);

    return num_lines;
}

Transaction* read_csv_into_transactions(char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Read the first line to get the number of transactions
    int num_transactions = number_of_lines_in_a_file(filename);

    // Allocate memory for the transactions
    Transaction *transactions = (Transaction *)malloc(num_transactions * sizeof(Transaction *));

    char line[256];
    int i = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *debtor = strtok(line, ",");
        char *creditor = strtok(NULL, ",");
        char *amount = strtok(NULL, ",");

        strcpy(transactions[i].debtor, debtor);
        strcpy(transactions[i].creditor, creditor);
        transactions[i].amount = atoi(amount);

        i++;
    }


    // Close the file
    fclose(fp);

    return transactions;    
}


int main(int argc, char** argv) {
    // Example transactions, replace with your own data

    Transaction* transactions;

    int num_transactions = number_of_lines_in_a_file(argv[1]);

    printf("Original number of transactions: %d\n", num_transactions);

    transactions = read_csv_into_transactions(argv[1]);

    min_transactions(transactions, num_transactions);

    free(transactions);
    
    return 0;
}

