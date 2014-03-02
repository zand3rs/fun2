
#include <iostream>

#include "libfuc.h"
#include "httpclient.hpp"
#include "pugixml.hpp"


using namespace std;

int main()
{
    HttpClient hc;
    int http_code = hc.httpGet("http://localhost/zander/soap.php", 30);
    string http_body = hc.getResponseBody();

    cout << "http_code: " << http_code << ", http_body: " << http_body << endl;
    
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(http_body.c_str());

    cout << "result: " << result << ", result.status: " << result.status << ", result.description: " << result.description() << endl;

    try {
        pugi::xml_node msisdn_node = doc
            .child("soapenv:Envelope")
            .child("soapenv:Body")
            .child("se:provisionResponse")
            .child("response")
            .child("elemen")
            .child("msisdn");

        cout << "soap:msisdn_node: " << msisdn_node << endl;
        if (msisdn_node) {
            cout << "soap:msisdn_node.child_value: " << msisdn_node.child_value() << endl;
        }
        cout << "soap:msisdn: " << atoi(msisdn_node.child_value()) << endl;
    } catch (...) {
        cout << "soap:msisdn: not found!" << endl;
    }

    
    return 0;
}
