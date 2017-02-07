#include "httpclient.hpp"
#include "libfuc.h"

int main(int argc, char* argv[]) {
    HttpClient hc;
    int http_code = hc.httpGet("http://urlecho.appspot.com/echo", 10);

    printf("response:\n status: %d\n headers: %s\n body: %s\n",
            http_code, hc.getResponseHeaders(), hc.getResponseBody());

    char buf[512];
    snprintf(buf, sizeof(buf), "%s", hc.getResponseHeaders());

    char* token;
    char* pbuf;

    //pbuf = strstr(buf, "Content-Type");
    pbuf = strstr(buf, "Cache-Control");
    if (pbuf) {
        token = strtok_r(NULL, ":\r\n", &pbuf);
        printf("token: [%s]\n", token);
        token = strtok_r(NULL, ":\r\n", &pbuf);
        printf("token: [%s]\n", token);
    }

    char location[64];
    snprintf(location, sizeof(location), "%s", token);

    printf("location: [%s]\n", location);
    str_trim(location);
    printf("location: [%s]\n", location);

    return 0;
}

