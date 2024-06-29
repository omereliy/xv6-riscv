#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

#define MAX_PRIMES 100


int is_prime(int n) {
    // Simple primality test
    if (n <= 1) return 0; // 1 is not prime
    if (n <= 3) return 1; // 2 and 3 are prime
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

void generator_process(int gen_to_checker, int checker_to_printer) {
    int num = 2;

    while (1) {
        channel_put(gen_to_checker, num);
        num++;
    }
}

void checker_process(int gen_to_checker, int checker_to_printer) {
    int num;
    int prime_count = 0;

    while (1) {
        if (channel_take(gen_to_checker, &num) == -1) {
            // Handle channel destruction
            break;
        }
        
        if (is_prime(num)) {
            channel_put(checker_to_printer, num);
            prime_count++;
            if (prime_count >= MAX_PRIMES) {
                // Signal shutdown
                channel_destroy(gen_to_checker);
                channel_destroy(checker_to_printer);
                break;
            }
        }
    }
}

void printer_process(int checker_to_printer) {
    int prime;
    int prime_count = 0;

    while (1) {
        if (channel_take(checker_to_printer, &prime) == -1) {
            // Handle channel destruction
            break;
        }

        printf("Prime num %d found: %d\n", prime_count+1, prime);
        prime_count++;
        if (prime_count >= MAX_PRIMES) {
            // Signal shutdown
            channel_destroy(checker_to_printer);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    int num_checkers = 3; // Default number of checker processes
    if (argc > 1) {
        num_checkers = atoi(argv[1]);
    }

    // create channels
    int gen_to_checker = channel_create();
    int checker_to_printer = channel_create();

    // Fork processes
    int pid;
    for (int i = 0; i < num_checkers; i++) {
        pid = fork();
        if (pid == 0) {
            checker_process(gen_to_checker, checker_to_printer);
            exit(0);
        } else if (pid < 0) {
            printf("Fork failed");
            exit(1);
        }
    }

    pid = fork();
    if (pid == 0) {
        generator_process(gen_to_checker, checker_to_printer);
        exit(0);
    } else if (pid < 0) {
        printf("Fork failed");
        exit(1);
    }

    pid = fork();
    if (pid == 0) {
        printer_process(checker_to_printer);
        exit(0);
    } else if (pid < 0) {
        printf("Fork failed");
        exit(1);
    }

    // Wait for all processes to terminate
    int status;
    while (wait(&status) > 0);

    // Cleanup and exit
    printf("All processes terminated. Exiting...\n");
    return 0;
}
