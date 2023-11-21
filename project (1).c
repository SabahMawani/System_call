#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
// prices of flavours and toppings

#define fPrice1 1.05
#define fPrice2 2.00
#define fPrice3 1.67
#define fPrice4 1.70
#define fPrice5 1.95

#define tPrice1 0.8
#define tPrice2 0.5
#define tPrice3 1.0

// resources
int remainingTickets = 30, _flavors[5] = {30, 30, 30, 30, 30}, _toppings[3] = {30, 30, 30};
char *flavName[5] = {"Vanilla", "Chocolate", "Cookies n Cream", "Chocolate Mousse Royale", "Cotton Candy"};
char *topName[3] = {"Chopped Almonds", "Rainbow Sprinkles", "Chocolate Chips"};
double totalBill = 0.0;

// semaphores declaration
// counters [ Only one person can enter the counter at a time ]
sem_t ticketC, flavorC, toppingC, paymentC;

// function prototypes
void *iceCream(void *_id);
void showMenu();

// main
int main()
{
    int customers;
    system("clear");
    showMenu(1);
    while(1)
    {
    int n;
    printf("\n\n\t\t\tEnter your choice: ");
    scanf("%d", &n);
    sleep(1);
    system("clear");
    if(n == 0 || remainingTickets ==0)
	{
        exit(0);
    }
    printf("\n\nEnter Number Of Customers [ %d  Tickets Remaining ]: ", remainingTickets);
    do{
        scanf("%d", &customers);
        if((customers > remainingTickets) || (customers <= 0)){
            printf("\nNumber of Customers is out of range!\nPlease enter customers within the range: ");
        }
    }while((customers > remainingTickets) || (customers <= 0));
    printf("\n\n");
    int _id[customers];

    for (int i = 0; i < customers; i++)
    {
        _id[i] = i;
    }

    // semaphore initialization - START
    sem_init(&ticketC, 0, 1);

    sem_init(&flavorC, 0, 1);

    sem_init(&toppingC, 0, 1);

    sem_init(&paymentC, 0, 1);

    // semaphore initialization - END

    // multithreading region - START
    pthread_t _customer[customers];

    for (int i = 0; i < customers; i++)
    {
        pthread_create(&_customer[i], 0, &iceCream, (void *)&_id[i]);
    }

    for (int i = 0; i < customers; i++)
    {
        pthread_join(_customer[i], NULL);
    }
    // multithreading region - END

    printf("\n\nBusiness Journal - At Closing\n\n");
    printf("Number Of Customers: %d", customers);
    printf("\nRevenue Generated: $ %.2f", totalBill);
    printf("\nTickets Remaining: %d\n\n", remainingTickets);

    // semaphore destroying - START

    sem_destroy(&ticketC);

    sem_destroy(&flavorC);

    sem_destroy(&toppingC);

    sem_destroy(&paymentC);

    }
    
    return 0;
}

void *iceCream(void *_id)
{
    int _ID = *(int *)_id, flavnum, n;
    double bill = 0.0;
    //ENTERING remainingTickets COUNTER
    sem_wait(&ticketC);
    if (remainingTickets <= 0)
    {
        printf("\nCustomer[ %d ]: Leaving Shop. [REASON]: Tickets Finished\n", _ID+1);
        sleep(1);
        pthread_exit(NULL);
    }

    remainingTickets--;

    printf("Customer [ %d ] Got Tickets.\n", _ID+1);

    // remainingTickets counter - EXIT

    sem_post(&ticketC);
    sleep(2);

    // flavours counter - ENTER
    sem_wait(&flavorC);
    sem_wait(&toppingC);
    sem_wait(&paymentC);
    printf("\n\n");
    printf("Customer [ %d ] entering the Flavor Counter\n", _ID+1);
    sleep(2);
    
    if (_flavors[0] <= 0 && _flavors[1] <= 0 && _flavors[2] <= 0 && _flavors[3] <= 0 && _flavors[4] <= 0)
    {
        printf("Customer [ %d ] Leaving Shop. [REASON]: Flavours Finished\n", _ID+1);
        sleep(1);
        pthread_exit(NULL);
    }
    else
    {
        showMenu(2);
        printf("Enter the number of flavors you want to add [Max 5]: ");
        scanf("%d", &n);
        while (n <= 0 || n > 5)
        {
            printf("Please enter the correct number of flavors here (1 to 5): ");
            scanf("%d", &n);
        }
        for (int i = 0; i < n; i++)
        {
            printf("\nEnter your desired flavor: ");
            scanf("%d", &flavnum);
            while (flavnum <= 0 || flavnum > 5 || _flavors[flavnum-1]<=0)
            {
                if(_flavors[flavnum-1]<=0)
                printf("Sorry %s is finished,Please Select another flavor: ",flavName[flavnum-1]);
                else{
                printf("Please enter the correct number of flavors here (1 to 5): ");
                }
                scanf("%d", &flavnum);
            }
            switch (flavnum)
            {
            case 1:
                if (_flavors[0] > 0)
                {
                    _flavors[0]--;
                    bill = bill + fPrice1;
                    printf("Customer [ %d ] Got %s Flavor.\n", _ID+1, flavName[flavnum-1]);
                    sleep(1);
                }

                break;
            case 2:

                if (_flavors[1] > 0)
                {
                    _flavors[1]--;
                    bill = bill + fPrice2;
                    printf("Customer [ %d ] Got %s Flavor.\n", _ID+1, flavName[flavnum-1]);
                    sleep(1);
                }

                break;
            case 3:

                if (_flavors[2] > 0)
                {
                    _flavors[2]--;
                    bill = bill + fPrice3;
                    printf("Customer [ %d ] Got %s Flavor.\n", _ID+1, flavName[flavnum-1]);
                    sleep(1);
                }
                break;
            case 4:

                if (_flavors[3] > 0)
                {
                    _flavors[3]--;
                    bill = bill + fPrice4;
                    printf("Customer [ %d ] Got %s Flavor.\n", _ID+1, flavName[flavnum-1]);
                    sleep(1);
                }
                break;
            case 5:

                if (_flavors[4] > 0)
                {
                    _flavors[4]--;
                    bill = bill + fPrice5;
                    printf("Customer [ %d ] Got %s Flavor.\n", _ID+1, flavName[flavnum-1]);
                    sleep(1);
                }
                break;
            }
        }
    }

    printf("\nCustomer [ %d ] Got Flavour(s). Leaving Flavor Counter\n", _ID+1);
    sem_post(&toppingC);
    sem_post(&paymentC);
    sem_post(&flavorC);
    // flavors counter - EXIT


    // toppings counter - ENTER
    sem_wait(&toppingC);
    sem_wait(&paymentC);
    if (_toppings[0] <= 0 && _toppings[1] <= 0 && _toppings[2] <= 0)
    {
        printf("Customer [ %d ] Leaving Shop. [REASON]: Toppings Finished\n", _ID+1);
        sleep(1);
        pthread_exit(NULL);
    }
    printf("\n\nCustomer [ %d ] entering the Toppings Counter\n", _ID+1);

    // topping1
    showMenu(3);
    int t;
    
    printf("\n\nDo you want to add toppings\n1.Yes\n2.No\nEnter your choice: ");
    scanf("%d",&t);
    while(t<1 || t>2)
    {
        printf("Enter a valid option: ");
        scanf("%d",&t);
    }

    if(t==2)
    {
        printf("No Toppings added\n");
        goto skip;
    }


    printf("Enter number of toppings you want to add: ");
    int numTopp;
    scanf("%d", &numTopp);

    while(numTopp <=0 || numTopp > 3){
        printf("Sorry! We don't offer this topping. Please enter a valid topping: ");
        scanf("%d", &numTopp);
    }
    for(int i=0; i<numTopp; i++){
        printf("\nEnter your desired Toppings: ");
        int choice;
        scanf("%d", &choice);
        while(_toppings[choice-1] <=0)
        {
            printf("Sorry! %s topping is finished,Please select another topping: ",topName[choice-1]);
            scanf("%d",&choice);
        }
        switch(choice){
            case 1:
                _toppings[0]--;
                bill = bill + tPrice1;
                printf("Customer [ %d ] Got %s Topping.\n", _ID+1, topName[choice-1]);
                sleep(1);
                break;
            case 2:
                _toppings[1]--;
                bill = bill + tPrice2;
                printf("Customer [ %d ] Got %s Topping.\n", _ID+1, topName[choice-1]);
                sleep(1);
                break;
            case 3:
                _toppings[2]--;
                bill = bill + tPrice3;
                printf("Customer [ %d ] Got %s Topping.\n", _ID+1, topName[choice-1]);
                sleep(1);
                break;
        }
    }
    skip:
    printf("Customer [ %d ] Leaving Topping Counter.\n", _ID+1);
    sem_post(&paymentC);
    sem_post(&toppingC);
    // toppings counter - EXIT

    sleep(2);

    // payments counter - ENTER
    printf("\n\n\t\t**********PAYMENT COUNTER**********\n");
    sem_wait(&paymentC);
    printf("\n\tTicket Number %d entering the Payment Counter:\n", _ID+1);
    totalBill = totalBill + bill;

    // printf("\nCustomer[%d]: Billed: $ %.2f.\n", _ID+1, bill);
    char a[300];
    sprintf(a, "\tBilled: $ %.2f.\n", bill);
    printf("\tCustomer [ %d ] %s", _ID+1, a);

    sem_post(&paymentC);
    // payments counter - EXIT
    // printf("\nCustomer[%d]: Leaving Ice-Cream Shop.\n\n", _ID+1);
    printf("\tCustomer [ %d ] Leaving Ice-Cream Shop.\n\n", _ID+1);

    return NULL;
}

void showMenu(int i)
{
	syscall(440,i);
     if(i==1)
     system("dmesg | tail -40");
      else if(i==2)
      system("dmesg | tail -6");
      else
      system("dmesg | tail -4");
}
