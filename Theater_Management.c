#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

char th_names[5][20] = {"CityMall", "Miraj", "MayaMandir", "Mridang", "PVR"};
char show_names[5][20] = {"I", "Dangal", "3 Idiots", "Boss", "DDLJ"}

struct seats {
    int seatno;
    bool isbooked;
    int price;
};

struct shows {
    char name[20];
    struct seats* seat[10];
};

struct theaters {
    char name[20];
    struct shows* show[5];
};

struct cities {
    char name[20];
    struct theaters* theater[5];
};

struct country {
    char name[10];
    struct cities* city;
};

struct country* india;

void* create_tree() {
    india = malloc(sizeof(struct country));
    strcpy(india->name, "India");
    struct cities* ahmd = (struct cities*)malloc(sizeof(struct cities));
    strcpy(ahmd->name, "Ahmedabad");
    india->city = ahmd;

    for (int i = 0; i < 5; i++) {
        struct theaters* new_th = (struct theaters*)malloc(sizeof(struct theaters));
        strcpy(new_th->name, th_names[i]);
        ahmd->theater[i] = new_th;
    }

    FILE* file = fopen("Th_data.txt", "w");
    fprintf(file, "%s\n", india->name);
    fprintf(file, "%s\n", india->city->name);

    for (int i = 0; i < 5; i++) {
        fprintf(file, "%s\n", india->city->theater[i]->name);
        for (int j = 0; j < 5; j++) {
            struct shows* new_show = (struct shows*)malloc(sizeof(struct shows));
            strcpy(new_show->name, show_names[j]);
            ahmd->theater[i]->show[j] = new_show;
            fprintf(file, "--%s\n", india->city->theater[i]->show[j]->name);

            for (int k = 0; k < 10; k++) {
                struct seats* seat = (struct seats*)malloc(sizeof(struct seats));
                seat->seatno = k + 1;
                seat->isbooked = false;

                if (k < 3)
                    seat->price = 80;
                else if (k < 6 && k >= 3)
                    seat->price = 100;
                else
                    seat->price = 150;

                new_show->seat[k] = seat;
                fprintf(file, "    |-seatno.%d, status %s, price %d\n", new_show->seat[k]->seatno,
                        new_show->seat[k]->isbooked ? "Booked" : "Available", new_show->seat[k]->price);
            }
        }
    }
    fclose(file);
}

void update_info() {
    FILE* file = fopen("Th_data.txt", "w");
    fprintf(file, "%s\n", india->name);
    fprintf(file, "%s\n", india->city->name);

    for (int i = 0; i < 5; i++) {
        fprintf(file, "%s\n", india->city->theater[i]->name);
        for (int j = 0; j < 5; j++) {
            fprintf(file, "--%s\n", india->city->theater[i]->show[j]->name);
            for (int k = 0; k < 10; k++) {
                bool status = india->city->theater[i]->show[j]->seat[k]->isbooked;
                fprintf(file, "    |-seatno.%d, status %s, price %d\n",
                        india->city->theater[i]->show[j]->seat[k]->seatno,
                        status ? "Booked" : "Available",
                        india->city->theater[i]->show[j]->seat[k]->price);
            }
        }
    }
    fclose(file);
}

void generate_ticket(struct theaters* sel_theater, struct shows* sel_show, struct seats* sel_seat, int seat_numbers[], int numseats, int tprice) {
    printf("theater_name %s\n", sel_theater->name);
    printf("show_name %s\n", sel_show->name);

    for (int i = 0; i < numseats; i++) {
        printf("seat no. are: %d\n", seat_numbers[i]);
    }
    printf("Generated Ticket Price %d\n", tprice);
}

void book_tkt() {
    char theater_name[20];
    char show_name[20];
    int num_seats, tprice=0;

    printf("enter the theater name: ");
    scanf("%s", theater_name);

    printf("enter the show name: ");
    scanf("%s", show_name);

    printf("enter the number of seats to book(Min:1 & MAX:10): ");
    scanf("%d", &num_seats); 
    if(num_seats<1 || num_seats>10)
    { 
        printf("Invalid Input\nPlease Enter number between 1-10\n"); 
        printf("enter the number of seats to book(Min:1 & MAX:10): "); 
        scanf("%d", &num_seats); 
    }

    struct theaters* sel_theater = NULL;
    struct shows* sel_show = NULL;
    int tflag = 0;
    int sflag = 0;
    struct seats* sel_seat;

    for (int i = 0; i < 5; i++) {
        if (strcmp(india->city->theater[i]->name, theater_name) == 0) {
            sel_theater = india->city->theater[i];
            tflag = 1;
            for (int j = 0; j < 5; j++) {
                if (strcmp(sel_theater->show[j]->name, show_name) == 0) {
                    sel_show = sel_theater->show[j];
                    sflag = 1;
                    break;
                }
            }
            if (sflag == 1)
                break;
        }
    }

    if (tflag == 0) {
        printf("Invalid Theater Name!!!\n");
    } else if (sflag == 0) {
        printf("Invalid Show name!!!\n");
    } else {
        int seat_numbers[num_seats];
        printf("Seat No.(1-3): Rs. 80/- || Seat No.(4-6): Rs. 100/- || Seat No.(7-10): Rs. 150/-\n");


        for (int i = 0; i < num_seats; i++) {
            int seat_num;
            bool valid_input = false;

            while (!valid_input) {
                printf("Enter seat number you want to book: ");
                scanf("%d", &seat_num);

                if (seat_num < 1 || seat_num > 10) {
                    printf("Invalid seat number. Please enter a number between 1 and 10.\n");
                } else if (sel_show->seat[seat_num - 1]->isbooked) {
                    printf("Seat %d is already booked. Please choose another seat.\n", seat_num);
                } else {
                    sel_seat = sel_show->seat[seat_num - 1];
                    sel_seat->isbooked = true;
                    tprice += sel_seat->price;
                    seat_numbers[i] = seat_num;
                    valid_input = true;
                }
            }
        }
        generate_ticket(sel_theater, sel_show, sel_seat, seat_numbers, num_seats, tprice);
        update_info();
    }
}

void cancel_tkt() {
    char theater_name[20];
    char show_name[20];
    int num_seats;

    printf("Enter the theater name: ");
    scanf("%s", theater_name);

    printf("Enter the show name: ");
    scanf("%s", show_name);

    printf("Enter the number of seats to cancel: ");
    scanf("%d", &num_seats);

    struct theaters* sel_theater = NULL;
    struct shows* sel_show = NULL;
    int tflag = 0;
    int sflag = 0;

    for (int i = 0; i < 5; i++) {
        if (strcmp(india->city->theater[i]->name, theater_name) == 0) {
            sel_theater = india->city->theater[i];
            tflag = 1;
            for (int j = 0; j < 5; j++) {
                if (strcmp(sel_theater->show[j]->name, show_name) == 0) {
                    sel_show = sel_theater->show[j];
                    sflag = 1;
                    break;
                }
            }
            if (sflag == 1)
                break;
        }
    }

    if (tflag == 0) {
        printf("Invalid Theater Name!!!\n");
    } else if (sflag == 0) {
        printf("Invalid Show name!!!\n");
    } else {
        printf("Entering Cancellation Process ...\n");
        int cancel_count = 0;
        sleep(1);

        for (int i = 0; i < num_seats; i++) {
            int seat_num;
            bool valid_input = false;

            while (!valid_input) {
                printf("Enter the seat number to cancel: ");
                scanf("%d", &seat_num);

                if (seat_num < 1 || seat_num > 10) {
                    printf("Invalid seat number. Please enter a number between 1 and 10.\n");
                } else if (!sel_show->seat[seat_num - 1]->isbooked) {
                    printf("Seat %d is not booked. Please choose another seat.\n", seat_num);
                } else {
                    sel_show->seat[seat_num - 1]->isbooked = false;
                    printf(" Cancelled Seat Number: %d\n", sel_show->seat[seat_num - 1]->seatno );
                    cancel_count++;
                    valid_input = true;
                }
            }
        }
        if (cancel_count > 0) {
            update_info();
            printf("Ticket cancellation successful!\n");
        } else {
            printf("No seats were cancelled.\n");
        }
    }
}
int main() {
    printf("Theater Names: ");
    for(int i=0;i<5;i++){
        printf("%s, ", th_names[i]);
    }
    printf("\n\nShow Names: ");
    for(int i=0;i<5;i++){
        printf("%s, ", show_names[i]);
    }
    printf("\n\nEach Show has 10 Available seats\n\n");

    pthread_t tid1;
    pthread_create(&tid1, NULL, create_tree, NULL);
    pthread_join(tid1, NULL);

    while(1){
        int choice;
        printf("choice-->  Action\n");
        printf("  1   -->Book Ticket\n");
        printf("  2   -->Cancel Ticket\n");
        printf("  3   -->Reset Tree\n");
        printf("  0   -->Exit\n\n");
        printf("Enter your choice:");
        scanf("%d",&choice);
        if(choice==1)
        {
            book_tkt();
        }
        else if(choice==2)
        {
            cancel_tkt();
        }
        else if(choice==3)
        {
            free(india);
            create_tree();
        }
        else
            return 0;
    }
        return 0;
}