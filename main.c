#include "stdio.h"
#include "user_level.h"
#include <stdlib.h>
#include <string.h>

// Function to initialize a recipt
void initialize_recipt(recipt *rcpt, int num_items) {
    rcpt->n = num_items; // Set the number of items
    rcpt->items = (item *)malloc(num_items * sizeof(item)); // Allocate memory for items
    if (rcpt->items == NULL) {
        perror("Failed to allocate memory for items");
        exit(EXIT_FAILURE);
    }

    // Initialize each item
    for (int i = 0; i < num_items; i++) {
        snprintf(rcpt->items[i].name, sizeof(rcpt->items[i].name), "Item%d", i + 1); // Default name
        rcpt->items[i].price = (i + 1) * 10.0; // Example price
        rcpt->items[i].score = 0.0;           // Default score
    }
}

// Function to free memory allocated for a recipt
void free_recipt(recipt *rcpt) {
    free(rcpt->items); // Free the allocated memory for items
    rcpt->items = NULL;
}

int main() {
    recipt rcpt[3]; // Array of 3 recipt structs

    // Initialize each recipt with a different number of items
    initialize_recipt(&rcpt[0], 5); // 5 items in the first store
    initialize_recipt(&rcpt[1], 3); // 3 items in the second store
    initialize_recipt(&rcpt[2], 4); // 4 items in the third store

    // Print the initialized data
    for (int i = 0; i < 3; i++) {
        printf("Recipt %d:\n", i + 1);
        for (int j = 0; j < rcpt[i].n; j++) {
            printf("  Name: %s, Price: %.2lf, Score: %.2lf\n",
                   rcpt[i].items[j].name, rcpt[i].items[j].price, rcpt[i].items[j].score);
        }
    }

    // Free memory for each recipt
    for (int i = 0; i < 3; i++) {
        free_recipt(&rcpt[i]);
    }

    return 0;
}
