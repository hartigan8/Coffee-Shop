#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define CUSTOMER_NUMBER 25

#define CUSTOMER_ARRIVAL_TIME_MIN 1
#define CUSTOMER_ARRIVAL_TIME_MAX 3

#define REGISTER_NUMBER 5

#define COFFEE_TIME_MIN 2
#define COFFEE_TIME_MAX 5

sem_t semaphore;
int registers[REGISTER_NUMBER]; // registers
int registerPointer = 0; // points to empty register

void* buyCoffee(void* customerId) {
    
    // To use id it's casted
    int customerID = *(int*)customerId;
    
    
    unsigned int coffeeTime = COFFEE_TIME_MIN + rand() % (COFFEE_TIME_MAX - COFFEE_TIME_MIN + 1);
    
    // Customer is inside so semaphore called
    sem_wait(&semaphore);
    // To store current customer buying from which register.
    int registerNumber;
    
    // Available register check.
    while (1) {
        if(registers[registerPointer] == 0){
            // Marking as busy.
            registers[registerPointer] = 1;
            // Assigning which register.
            registerNumber = registerPointer;
            break;
        }
        else{
            // Increment pointer if register is full.
            registerPointer = (registerPointer + 1) % REGISTER_NUMBER;
        }
    }
    
    
    printf("CUSTOMER %d GOES TO REGISTER %d\n", customerID, registerNumber);
    // Spending time in register.
    sleep(coffeeTime);
    printf("CUSTOMER %d FINISHED BUYING FROM REGISTER %d AFTER %d SECONDS.\n", customerID, registerNumber, coffeeTime);
    // Customer leaves so semaphore relased.
    sem_post(&semaphore);
    // Marking the register as available.
    registers[registerNumber] = 0;
    // Relasing allocated memory.
    free(customerId);
    return 0;
}

int main(int argc, char* argv[]) {
    
    
    // Semaphore initialized with value of register number because customers have to wait for registers
    sem_init(&semaphore, 0, REGISTER_NUMBER);
    // Threads initialized as customers.
    pthread_t threads[CUSTOMER_NUMBER];
    // Random initialized.
    srand(time(NULL));
    
    // creating customers
    int i;
    for (i = 0; i < CUSTOMER_NUMBER; i++) {
        
        // To store the id of a customer a location is cleared then id is stored.
        int* customerId = calloc(1, sizeof(int));
        *customerId = i;
        
        // Generating random value then sleeping. Because there should be a delay beetween customers.
        unsigned int creationTime = CUSTOMER_ARRIVAL_TIME_MIN + rand() % (CUSTOMER_ARRIVAL_TIME_MAX - CUSTOMER_ARRIVAL_TIME_MIN + 1);
        sleep(creationTime);
        
        // Creating threads (customers).
        printf("CUSTOMER %d IS CREATED AFTER %d SECONDS.\n", *customerId, creationTime);
        pthread_create(&threads[i], NULL, &buyCoffee, customerId);
        
    }
    
    // to wait all processes to finish
    for(i = 0; i < CUSTOMER_NUMBER; i++){
        pthread_join(threads[i], NULL);
    }
    
    sem_destroy(&semaphore);
    return 0;
}
