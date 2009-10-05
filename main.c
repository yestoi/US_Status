#include "curl.h"

int main(int argc, char **argv)
{
    char *data;

    data = getdata("http://undergroundsystems.org");

    //Do regex and notify code here
    printf("%s", data);
    //END

    if (data)
        free(data);

    return 0;
}


