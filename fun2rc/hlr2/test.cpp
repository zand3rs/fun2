#include "httpclient.hpp"
#include "libfuc.h"

int main(int argc, char* argv[]) {
    //HttpClient hc;
    //int http_code = hc.httpGet("http://urlecho.appspot.com/echo", 10);

    //printf("response:\n status: %d\n headers: %s\n body: %s\n",
    //        http_code, hc.getResponseHeaders(), hc.getResponseBody());

    //char buf[512];
    //snprintf(buf, sizeof(buf), "%s", hc.getResponseHeaders());

    char* token;
    char* pbuf;

    //pbuf = strstr(buf, "Content-Type");
    char buf[512] = "HTTP/1.1 307 Temporary Redirect\n"
        "Location: http://10.157.26.69:8001/00229898\n"
        "Server: Huawei web server\n"
        "Content-Type: text/xml; charset=\"utf-8\"\n"
        "Content-Length: 368\n";

    pbuf = strstr(buf, "Location");
    if (pbuf) {
        token = strtok_r(NULL, ":\r\n", &pbuf);
        printf("token: [%s]\n", token);
        token = strtok_r(NULL, "\r\n", &pbuf);
        printf("token: [%s]\n", token);
    }

    char location[64];
    snprintf(location, sizeof(location), "%s", token);

    printf("location: [%s]\n", location);
    str_trim(location);
    printf("location: [%s]\n", location);

    return 0;
}

