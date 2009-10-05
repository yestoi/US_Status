#include "curl.h"

char *getdata(char *website)
{
    CURL *curl;
    struct memstruct file; 

    file.mem = NULL; //what did your mother tell you about null pointers?
    file.size = 0;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://undergroundsystems.org");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&file);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return file.mem;
}

static size_t write_call(void *ptr, size_t size, size_t newmem, void *data)
{
    size_t realsize = size * newmem;
    struct memstruct *mem = (struct memstruct *)data;

    mem->mem = realloc(mem->mem, mem->size + realsize + 1);
    if (mem->mem) {
        memcpy(&(mem->mem[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->mem[mem->size] = 0;
    }
    return realsize;
}

