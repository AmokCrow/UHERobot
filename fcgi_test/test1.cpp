#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    int count = 0;
    printf("Content-type: text/html\r\n"
           "\r\n"
           "<title>CGI Hello!</title>"
           "<h1>CGI Hello!</h1>"
           "Request number %d running on host <i>%s</i>\n",
           ++count, getenv("SERVER_NAME"));
}
