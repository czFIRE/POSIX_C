#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char date[1024];
    char cmd[1024];

    printf("> ");
    scanf("%[^\n]", date);
    sprintf(cmd, "date -d '%s'", date);
    system(cmd);

    return EXIT_SUCCESS;
}
