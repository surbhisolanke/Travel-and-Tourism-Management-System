#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DESTINATIONS 5
#define TRANSPORTS 3
#define PACKAGES 3

struct Booking
{
    int bookingID;
    char name[50];
    char phone[15];
    int destination;
    int transport;
    int package;
    int persons;
    float subtotal;
    float discount;
    float cgst;
    float sgst;
    float total;
    struct Booking *next;
};

struct Booking *head = NULL;
int nextBookingID = 1001;

char *places[] = {"Goa", "Maharashtra", "Rajasthan", "Himachal Pradesh", "Kerala"};
char *transports[] = {"Flight", "Train", "Bus"};
char *packages[] = {"Silver", "Gold", "Diamond"};

/* Prices are per person. */
float destinationPrice[DESTINATIONS][TRANSPORTS] = {
    {5000.0f, 2000.0f, 1000.0f},
    {4000.0f, 1500.0f, 1000.0f},
    {6000.0f, 2500.0f, 1500.0f},
    {7000.0f, 3000.0f, 2000.0f},
    {5500.0f, 2000.0f, 1200.0f}
};

float packagePrice[PACKAGES] = {1000.0f, 2500.0f, 4000.0f};

int validPhone(const char phone[])
{
    int i;
    if (strlen(phone) != 10)
        return 0;

    for (i = 0; phone[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)phone[i]))
            return 0;
    }
    return 1;
}

void calculateBill(struct Booking *b)
{
    float taxableAmount;

    /* Convert to floating-point before multiplication to prevent integer overflow. */
    b->subtotal = (destinationPrice[b->destination][b->transport]
                  + packagePrice[b->package]) * (float)b->persons;

    /* Discount: 10% for 4 to 10 persons; 20% for more than 10 persons. */
    b->discount = 0.0f;
    if (b->persons > 10)
        b->discount = b->subtotal * 0.20f;
    else if (b->persons >= 4)
        b->discount = b->subtotal * 0.10f;

    taxableAmount = b->subtotal - b->discount;
    b->cgst = taxableAmount * 0.09f;
    b->sgst = taxableAmount * 0.09f;
    b->total = taxableAmount + b->cgst + b->sgst;
}

void printBill(struct Booking *b)
{
    printf("\n---------- BOOKING BILL ----------\n");
    printf("Booking ID : %d\n", b->bookingID);
    printf("Name       : %s\n", b->name);
    printf("Phone      : %s\n", b->phone);
    printf("Destination: %s\n", places[b->destination]);
    printf("Transport  : %s (Rs. %.2f/person)\n", transports[b->transport], destinationPrice[b->destination][b->transport]);
    printf("Package    : %s (Rs. %.2f/person)\n", packages[b->package], packagePrice[b->package]);
    printf("Persons    : %d\n", b->persons);
    printf("Subtotal   : Rs. %.2f\n", b->subtotal);
    printf("Discount   : Rs. %.2f\n", b->discount);
    printf("After Disc.: Rs. %.2f\n", b->subtotal - b->discount);
    printf("CGST (9%%)  : Rs. %.2f\n", b->cgst);
    printf("SGST (9%%)  : Rs. %.2f\n", b->sgst);
    printf("TOTAL      : Rs. %.2f\n", b->total);
    printf("----------------------------------\n");
}

void displayBooking(struct Booking *b)
{
    if (b == NULL)
        return;

    calculateBill(b);
    printBill(b);
}

struct Booking *findBookingByID(int id)
{
    struct Booking *temp;
    temp = head;
    while (temp != NULL)
    {
        if (temp->bookingID == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

int chooseOption(const char *title, char *options[], int count)
{
    int choice, i;

    printf("\n%s\n", title);
    for (i = 0; i < count; i++)
        printf("%d. %s\n", i + 1, options[i]);

    printf("Enter choice (0 to go back): ");
    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input.\n");
        while (getchar() != '\n')
            ;
        return -1;
    }

    if (choice == 0)
        return -1;
    if (choice < 1 || choice > count)
    {
        printf("Invalid choice.\n");
        return -1;
    }
    return choice - 1;
}

void bookTrip(void)
{
    struct Booking *b, *temp;
    int choice, i;

    b = (struct Booking *)calloc(1, sizeof(struct Booking));
    if (b == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    choice = chooseOption("Select Destination:", places, DESTINATIONS);
    if (choice < 0)
    {
        free(b);
        return;
    }
    b->destination = choice;

    printf("\nTransport prices for %s:\n", places[b->destination]);
    for (i = 0; i < TRANSPORTS; i++)
        printf("%d. %s - Rs. %.2f per person\n", i + 1, transports[i], destinationPrice[b->destination][i]);

    printf("Enter transport choice (0 to go back): ");
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
            ;
        free(b);
        printf("Invalid input.\n");
        return;
    }
    if (choice == 0)
    {
        free(b);
        return;
    }
    if (choice < 1 || choice > TRANSPORTS)
    {
        free(b);
        printf("Invalid choice.\n");
        return;
    }
    b->transport = choice - 1;

    printf("\nSelect Package:\n");
    for (i = 0; i < PACKAGES; i++)
        printf("%d. %s - Rs. %.2f per person\n", i + 1, packages[i], packagePrice[i]);

    printf("Enter package choice (0 to go back): ");
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
            ;
        free(b);
        printf("Invalid input.\n");
        return;
    }
    if (choice == 0)
    {
        free(b);
        return;
    }
    if (choice < 1 || choice > PACKAGES)
    {
        free(b);
        printf("Invalid choice.\n");
        return;
    }
    b->package = choice - 1;

    printf("Enter customer name: ");
    if (scanf(" %49[^\n]", b->name) != 1)
    {
        free(b);
        printf("Invalid name.\n");
        return;
    }

    do
    {
        printf("Enter 10-digit phone number (0 to cancel): ");
        if (scanf("%14s", b->phone) != 1)
        {
            free(b);
            return;
        }
        if (strcmp(b->phone, "0") == 0)
        {
            free(b);
            printf("Booking cancelled.\n");
            return;
        }
        if (!validPhone(b->phone))
            printf("Enter exactly 10 digits.\n");
    } while (!validPhone(b->phone));

    printf("Enter number of persons (0 to cancel): ");
    if (scanf("%d", &b->persons) != 1)
    {
        while (getchar() != '\n')
            ;
        free(b);
        printf("Invalid input.\n");
        return;
    }
    if (b->persons <= 0)
    {
        free(b);
        printf("Booking cancelled.\n");
        return;
    }

    b->bookingID = nextBookingID++;
    calculateBill(b);
    printBill(b);

    printf("\nConfirm booking? (1 = Yes, 0 = No): ");
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
            ;
        free(b);
        printf("Invalid input.\n");
        return;
    }
    if (choice != 1)
    {
        free(b);
        printf("Booking not saved.\n");
        return;
    }

    b->next = NULL;
    if (head == NULL)
        head = b;
    else
    {
        temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = b;
    }
    printf("Booking confirmed! Booking ID: %d\n", b->bookingID);
}

void viewBookings(void)
{
    struct Booking *temp;
    temp = head;

    if (temp == NULL)
    {
        printf("No bookings available.\n");
        return;
    }
    while (temp != NULL)
    {
        displayBooking(temp);
        temp = temp->next;
    }
}

void searchBooking(void)
{
    int id;
    struct Booking *b;

    printf("Enter Booking ID (0 to go back): ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        return;
    }
    if (id == 0)
        return;

    b = findBookingByID(id);
    if (b == NULL)
        printf("Booking not found.\n");
    else
        displayBooking(b);
}

void updateBooking(void)
{
    int id, choice, newValue, i;
    struct Booking *b;

    printf("Enter Booking ID to update (0 to go back): ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        return;
    }
    if (id == 0)
        return;

    b = findBookingByID(id);
    if (b == NULL)
    {
        printf("Booking not found.\n");
        return;
    }

    displayBooking(b);
    printf("\nWhat would you like to update?\n");
    printf("1. Destination\n2. Transport\n3. Package\n4. Number of Persons\n0. Go Back\nEnter choice: ");
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        return;
    }
    if (choice == 0)
        return;

    switch (choice)
    {
        case 1:
            newValue = chooseOption("Select New Destination:", places, DESTINATIONS);
            if (newValue < 0)
                return;
            b->destination = newValue;
            break;

        case 2:
            printf("Transport prices for %s:\n", places[b->destination]);
            for (i = 0; i < TRANSPORTS; i++)
                printf("%d. %s - Rs. %.2f per person\n", i + 1, transports[i], destinationPrice[b->destination][i]);
            printf("Enter new transport (0 to go back): ");
            if (scanf("%d", &newValue) != 1)
            {
                while (getchar() != '\n')
                    ;
                printf("Invalid input.\n");
                return;
            }
            if (newValue == 0)
                return;
            if (newValue < 1 || newValue > TRANSPORTS)
            {
                printf("Invalid choice.\n");
                return;
            }
            b->transport = newValue - 1;
            break;

        case 3:
            newValue = chooseOption("Select New Package:", packages, PACKAGES);
            if (newValue < 0)
                return;
            b->package = newValue;
            break;

        case 4:
            printf("Enter new number of persons: ");
            if (scanf("%d", &newValue) != 1)
            {
                while (getchar() != '\n')
                    ;
                printf("Invalid input.\n");
                return;
            }
            if (newValue <= 0)
            {
                printf("Number of persons must be greater than zero.\n");
                return;
            }
            b->persons = newValue;
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    calculateBill(b);
    printf("\nBooking updated successfully. Updated bill:\n");
    printBill(b);
}

void cancelBooking(void)
{
    struct Booking *temp, *prev;
    int id, confirm;

    temp = head;
    prev = NULL;

    printf("Enter Booking ID to cancel (0 to go back): ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        return;
    }
    if (id == 0)
        return;

    while (temp != NULL && temp->bookingID != id)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
    {
        printf("Booking not found.\n");
        return;
    }

    displayBooking(temp);
    printf("Are you sure you want to cancel? (1 = Yes, 0 = No): ");
    if (scanf("%d", &confirm) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        return;
    }
    if (confirm != 1)
    {
        printf("Cancellation aborted.\n");
        return;
    }

    if (prev == NULL)
        head = temp->next;
    else
        prev->next = temp->next;

    free(temp);
    printf("Booking cancelled successfully.\n");
}

int main(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("       TRAVEL AND TOURISM SYSTEM\n");
        printf("========================================\n");
        printf("1. Book a Trip\n");
        printf("2. View Booking\n");
        printf("3. Update Booking\n");
        printf("4. Search Booking\n");
        printf("5. Cancel Booking\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n')
                ;
            choice = 0;
            continue;
        }

        switch (choice)
        {
            case 1:
                bookTrip();
                break;
            case 2:
                viewBookings();
                break;
            case 3:
                updateBooking();
                break;
            case 4:
                searchBooking();
                break;
            case 5:
                cancelBooking();
                break;
            case 6:
                printf("Thank you for using Travel and Tourism System!\n");
                break;
            default:
                printf("Invalid choice. Please select 1 to 6.\n");
        }
    } while (choice != 6);

    while (head != NULL)
    {
        struct Booking *temp;
        temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
