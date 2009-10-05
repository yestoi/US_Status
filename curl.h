#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static size_t write_call(void *ptr, size_t size, size_t newmem, void *data);
char *getdata(char *website); 

struct memstruct {
    char *mem;
    size_t size;
};
