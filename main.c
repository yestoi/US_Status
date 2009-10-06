#include "curl.h"

#define SITE "http://undergroundsystems.org"

int main(int argc, char **argv)
{
    char *data;

    data = getdata(SITE);

    //Do regex and notify code here
    printf("%s\n", data);
    //END

    if (data)
        free(data);

    return 0;
}


