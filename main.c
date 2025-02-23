/* Project 0 - Input Validation and Buffer Overruns
 * Author: Joshua Miller
 * Purpose: Create a program that does not validate input and leads to a buffer
 *          overflow, and allows memory to directly accessed.  Also, write
 *          more secure versions of the functions that do not have these
 *          vulnerabilities.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> /* immediately exit the program with exit(0) */

#define MAX_USERS 10    /* maximum number of users allowed */
#define MAX_NAME_LEN 39 /* maximum number of letters in username allowed */
#define EXIT_VALUE 99   /* what to type to quit the loop */

void print_this_user_info(unsigned short userindex, char username[], unsigned short userpin, bool userIsAdmin);
bool get_user_preference();
int get_user_to_modify_vulnerable();
void change_pin_vulnerable(int user_i, unsigned short u_pin[], int new_pin);
int get_user_to_modify_more_secure(int current_num_users);
bool change_pin_more_secure(unsigned int *user_pin, int new_pin, int pin_length);


int validate_pin(int pin_length);

int main(void) {
    struct {
        unsigned short user_isAdmin[MAX_USERS];   /* an array, true if this user is an admin */
        unsigned short user_pin[MAX_USERS];       /* an array, user's PIN, in decimal form */
        char user_name[MAX_USERS][MAX_NAME_LEN]; /* an array of character strings */
    } user_data;                    /* declare one variable to hold all user information */
    int i = 0;                      /* loop counter */
    int num_users = 0;              /* how many users do we have? */
    int user_index = 0;             /* which user to work with */
    int new_pin = 0;                /* new value for pin */
    char buffer[256] = "";          /* read from the keyboard */
    bool vulnerable_mode = false;   /* user preference to run vulnerable functions, or not */
    bool success = false;           /* did the pin change succeed */
    int pin_length = 5;

    /******* set up default user accounts *******/
    /* zero out all memory in the user_data variable -- for each array */
    memset(user_data.user_isAdmin, 0, sizeof(user_data.user_isAdmin));
    memset(user_data.user_pin, 0, sizeof(user_data.user_pin));
    memset(user_data.user_name, '-', sizeof(user_data.user_name));   // this makes it easier to see in memory.
    /* user at index 0 is the administrator, with PIN 16962 */
    user_data.user_pin[0] = 16962; // 4242 in hex
    user_data.user_isAdmin[0] = true;
    strncpy(user_data.user_name[0], "ADMIN", strlen("ADMIN")+1);
    /* user at index 1 is the default user, with PIN  */
    user_data.user_pin[1] = 4369; // 1111 in hex
    user_data.user_isAdmin[1] = false;
    strncpy(user_data.user_name[1], "DEFAULT USER", strlen("DEFAULT USER")+1);
    /* we have 2 users so far */
    num_users = 2;
    
        for (i = 0; i < num_users; i++) {
            print_this_user_info(i, user_data.user_name[i],
                                 user_data.user_pin[i], user_data.user_isAdmin[i]);
        }
        printf("-------------\n");


    vulnerable_mode = get_user_preference();


    if (vulnerable_mode) {
        int vul_index = get_user_to_modify_vulnerable();

        printf("Enter a number: ");
        scanf("%d", &new_pin);
        printf("You entered: %d\n", new_pin);

        change_pin_vulnerable(vul_index, user_data.user_pin, new_pin);

        printf("new pin: %d\n", user_data.user_pin[vul_index]);
    }
    else {
        int secure_index = get_user_to_modify_more_secure(num_users);

        bool pinEntered = false;

        new_pin = validate_pin(pin_length);
        printf("Valid PIN! Thank you!\n");

        change_pin_more_secure(&user_data.user_pin[secure_index], new_pin, pin_length);
    }

    for (i = 0; i < num_users; i++) {
        print_this_user_info(i, user_data.user_name[i],
                             user_data.user_pin[i], user_data.user_isAdmin[i]);
    }
    printf("-------------\n");

    return 0;
}

void print_this_user_info(unsigned short userindex, char username[],
                          unsigned short userpin, bool userIsAdmin) {
    /* print one user at a time */
    printf("---User Index#%d---\nName: %s  \nPIN: %u  \tisAdmin: %i\n",
           userindex, username, userpin, userIsAdmin);
}


bool get_user_preference() {
    char buffer[256] = "";
    int selection = 0;

    printf("Answer 1 if you want vulnerable mode. Answer anything else if you want secure mode.");

    fgets(buffer, sizeof(buffer), stdin);

    sscanf(buffer, "%d", &selection);


    if (selection == 1) {
        return true;
    } else {
        return false;
    }
}

int get_user_to_modify_vulnerable(void) {
    char buffer[256] = "";
    int  desired_index = 0;

    printf("Enter your index: ");

    fgets(buffer, sizeof(buffer), stdin);

    sscanf(buffer, "%d", &desired_index);


    return desired_index;
}


void change_pin_vulnerable(int user_i, unsigned short u_pin[], int new_pin) {
    u_pin[user_i] = new_pin;
}

int get_user_to_modify_more_secure(int current_num_users) {
    bool exited = false;
    while (!exited) {
        char buffer[256] = "";
        int desired_index = -1;

        printf("Please enter a valid index to modify.\n");

        fgets(buffer, sizeof(buffer), stdin);

        sscanf(buffer, "%d", &desired_index);

        if (desired_index < current_num_users && desired_index >= 0) {
            exited = true;
            return (int)desired_index;
        }
        else {
            printf("Wrong answer.\n");
        }
    }

    return -1;
}

bool change_pin_more_secure(unsigned int *user_pin, int new_pin, int pin_length) {
    if (user_pin == NULL) {
        printf("Invalid pin location.\n");
        return false;
    }

    int temp = new_pin;
    int count = 0;

    while (temp > 0) {
        count++;
        temp /= 10;
    }

    if (new_pin == 0) {
        count = 1;
    }

    if (count != pin_length) {
        printf("Pin must be %d digits.\n", pin_length);
        return false;
    }

    *user_pin = (unsigned int)new_pin;
    printf("PIN successfully changed to %d.\n", *user_pin);
    return true;
}


int validate_pin(int pin_length) {
    char buffer[256];
    int new_pin = -1;

    while (true) {
        printf("Please enter your PIN: ");
        scanf("%s", buffer);

        // Verify the pin is only numbers
        bool valid = true;
        for (int i = 0; i < strlen(buffer); i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                valid = false;
                break;
            }
        }

        if (valid && strlen(buffer) == pin_length) {
            sscanf(buffer, "%d", &new_pin);
            return new_pin;
        } else {
            printf("Invalid PIN. Please try again.\n");
        }
    }
}