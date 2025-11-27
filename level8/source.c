#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
    char *auth = NULL;
    char *service = NULL;
    char input_buffer[128];
    char username[32];
    char service_name[125];

    while (1) {
        printf("%p, %p \n", auth, service);

        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            return 0;
        }

        if (strncmp(input_buffer, "auth ", 5) == 0) {
            auth = malloc(4);
            if (auth) {
                memset(auth, 0, 4);
                if (strlen(username) < 31) {
                    strcpy(auth, username);
                }
            }
        }
        if (strncmp(input_buffer, "reset", 5) == 0) {
            free(auth);
        }

        if (strncmp(input_buffer, "service", 6) == 0) {
            service = strdup(service_name);
        }

        if (strncmp(input_buffer, "login", 5) == 0) {
            if (*(int *)(auth + 32) == 0) {
                fwrite("Password:\n", 1, 10, stdout);
            } else {
                system("/bin/sh");
            }
        }
    }
}
