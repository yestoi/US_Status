#include <curl/curl.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>
#include <stdio.h>
#include <string.h>

#define SITE "undergroundsystems.org"

/* Normally globals are bad. In this case however I need it. I'm using a nasty 
 * recursive dom transversal to find the newest post. I would have to pass the flag
 * for each call and then have to worry about variable scope. To make it simple I just
 * made it global. Sue me. */
int flag=0;
int count=2;

uint write_func(char *input, uint size, uint newbuff, TidyBuffer *ouput)
{
    uint total;
    total = size * newbuff;
    tidyBufAppend(ouput, input, total);
    return total;
}

void chomp(const char *str)
{
    //C version of chomp from perl. Eh, it works...
    char *p;
    while (NULL != str && NULL != (p = strrchr(str, '\n'))) {
        *p = '\0';
    }
}

char *getNewPost(TidyDoc doc, TidyNode root)
{
    TidyNode child;
    char *str = (char *) malloc(sizeof(char));
    
    for (child = tidyGetChild(root); child; child = tidyGetNext(child))
    {
        ctmbstr name = tidyNodeGetName(child);
        //If it has a name it's a tag. We need to start somewhere so <td> works.
        if (name) {
            if (strcmp(name, "td") == 0) {
                TidyAttr attr;
                //Walk through the attribute list for a tag. Set flags for yoyo between tags/content
                for (attr = tidyAttrFirst(child); attr; attr=tidyAttrNext(attr)) {
                    if (flag == -1 && strcmp(tidyAttrValue(attr), "alt1") == 0) {
                        flag = 2;
                    }
                    if (strcmp(tidyAttrValue(attr), "20%") == 0) {
                        flag = 1;
                    }
                }
            }
        }
        
        //Anything else is cdata or text.
        else {
            if (flag > 0) {
                TidyBuffer buf;
                tidyBufInit(&buf);
                tidyNodeGetText(doc, child, &buf);
                chomp(buf.bp);

                if (strcmp(buf.bp, "Forum") == 0) {
                    flag = -1;
                }
                if (flag == 2) {
                    if (!strcmp(buf.bp, " ") == 0 && count) {
                        printf("%s\n", buf.bp);
                       // strcat(str, (char *)buf.bp);
                        //printf("%s\n", str);
                        --count;
                    }

                }
                tidyBufFree(&buf);
            }
        }
        getNewPost(doc, child);
    }
}

int main(int argc, char **argv)
{
    CURL *curl;
    TidyDoc doc;
    TidyBuffer data = {0};
    TidyBuffer error = {0};
    int err = 0;
    int flag = 0;
    char *point;
 
    doc = tidyCreate();
    tidyOptSetBool(doc, TidyForceOutput, yes);
    tidyOptSetInt(doc, TidyWrapLen, 4096);
    tidySetErrorBuffer(doc, &error); //put error messages into its buffer instead of stdout
    tidyBufInit(&data);   

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, SITE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

    err = curl_easy_perform(curl);
    if (err >= 0) {
        err = tidyParseBuffer(doc, &data);
        if (err >= 0) {
            err = tidyCleanAndRepair(doc);
            if (err >= 0) {
                getNewPost(doc, tidyGetRoot(doc));
            }
        }
    }

    curl_easy_cleanup(curl);
    tidyBufFree(&data);
    tidyBufFree(&error);
    tidyRelease(doc);
    return 0;
}
