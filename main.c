#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct to represent transaction information
typedef struct {
    char debtor[64];
    char creditor[64];
    int amount;
} Transaction;

// Struct to represent person information
typedef struct {
    char name[64];
    int balance;
} Person;

// Global variables

// Counter to keep track of the number of people, initialized to zero
// malloc / rellac will be used to allocate memory basis this counter
int people = 0; 


// person is a pointer to an array of Person 
Person *person;

// Function to return the minimum of two integers
int minimum(int a, int b) {
    return a < b ? a : b;
}

// Check if the person is present in the list of person array
int isPresent(char *name) { 
    for(int i = 0; i < people; i++) {
        if(strcmp(person[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Initializing the person array with the names of the people and zero balance
void initializePerson(Transaction* transactions, int num_transactions) {
    // Initialize balances for each person
    int index;

    // Iterate through the transactions and extract the debtor and creditor information
    // to create unique list of people
    
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

// Increase the balance of the person by the amount of the transaction
// if the person is a creditor
void increaseBalance(char *name, int amount) {
    for(int i = 0; i < people; i++) {
        if(strcmp(person[i].name, name) == 0) {
            person[i].balance += amount;
            break;
        }
    }
}   

// Decrease the balance of the person by the amount of the transaction
// if the person is a debtor
void decreaseBalance(char *name, int amount) {
    for(int i = 0; i < people; i++) {
        if(strcmp(person[i].name, name) == 0) {
            person[i].balance -= amount;
            break;
        }
    }
}

// Compare function for quicksort
// Sorts the person array by balance amount
// return line sorts in ascending order 
// if you do reverse in return line, it will sort in descending order
// But for our requirement, we need to sort in ascending order
int compare(const void *a, const void *b) {
    return ((Person *)a)->balance - ((Person *)b)->balance;
}


// Function to calculate the minimum number of transactions required to settle the debt
void optimum_transactions(Transaction *transactions, int num_transactions) {

    // Initialize balances unique list of people with their names and zero balance
    initializePerson(transactions, num_transactions);

    // Update balances for each person based on the transactions
    for(int i = 0; i < num_transactions; i++) {
        increaseBalance(transactions[i].creditor, transactions[i].amount);
        decreaseBalance(transactions[i].debtor, transactions[i].amount);
    }

    // Sort person array by balance using quicksort
    qsort(person, people, sizeof(Person), compare);

    // Pick the first and last person from the sorted array
    int i = 0;
    int j = people - 1;

    // Array to store the result transactions
    Transaction *transactions_list = (Transaction *)malloc(0 * sizeof(Transaction)); 

    // Optimal number of transactions required to settle the debt
    int num_result_transactions = 0;

    // Iterate through the sorted array and settle the debts
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
            
            // Update balances in the person array
            transactions_list = (Transaction *)realloc(transactions_list, (num_result_transactions + 1) * sizeof(Transaction));

            // Record the transaction
            // If min_transfer is positive, then debtor owes creditor
            // If min_transfer is negative, then we need to reverse the debtor and creditor
            if(min_transfer > 0) {
                transactions_list[num_result_transactions].amount = min_transfer;
                strcpy(transactions_list[num_result_transactions].debtor, debtor);
                strcpy(transactions_list[num_result_transactions].creditor, creditor);
            } else {
                transactions_list[num_result_transactions].amount = -min_transfer;
                strcpy(transactions_list[num_result_transactions].debtor, creditor);
                strcpy(transactions_list[num_result_transactions].creditor, debtor);
            }

            // Update balances in the person array
            num_result_transactions++;

        }

        // Check for zero balances and settle debts
        if (debtor_balance == 0) {
            i++;
        }
        if (creditor_balance == 0) {
            j--;
        }
    }
    

    printf("Optimized Number of transactions for settlemnt of debt: %d\n", num_result_transactions);

    // Print the result transactions
    for (int k = 0; k < num_result_transactions; ++k) {
        printf("%s owes %s %d\n", transactions_list[k].debtor, transactions_list[k].creditor, transactions_list[k].amount);
    }


    // Free allocated memory
    free(person);
    free(transactions_list);
    free(transactions);
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

    // Return array of transactions that have been read from csv file
    return transactions;    
}


int main(int argc, char** argv) {

    Transaction* transactions;

    // Check if the filename is provided as an argument
    // If not, then exit the program
    
    if (argc < 2) {
        printf("Please provide the csv file name after the executable\n");
        printf("Example: ./main transactions.csv\n");
        exit(1);
    }
    char* filename = argv[1];

    // Finds the number of transactions in the csv file
    // This is used to allocate memory for the transactions array
    int num_transactions = number_of_lines_in_a_file(filename);

    printf("Original number of transactions: %d\n", num_transactions);

    // Reads the transactions in the csv file into an array of transactions
    transactions = read_csv_into_transactions(filename);

    // Calculate the optimum number of transactions required to settle the debt
    optimum_transactions(transactions, num_transactions);
    
    return 0;
}

