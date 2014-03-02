<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:sset="http://BasicService_SSETLib/SSETService">
<soapenv:Header/>
<soapenv:Body>
    <sset:provision>
        <request>
            <element>
                <msisdn>639271002030</msisdn>
                <service>SMS</service>
                <serviceName>ROAMING</serviceName>
                <requestType>2</requestType>
                <shallow>false</shallow>
            </element>
        </request>
    </sset:provision>
</soapenv:Body>
</soapenv:Envelope>





<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/">
<soapenv:Body>
    <se:provisionResponse xmlns:se="http://BasicService_SSETLib/SSETService">
        <response>
            <element>
                <msisdn>639271002030</msisdn>
                <service>SMS</service>
                <serviceName>ROAMING</serviceName>
                <requestType>2</requestType>
                <address>12345</address>
                <shallow>false</shallow>
                <returnCode>0<returnCode>
            </element>
        </response>
    </se:provisionResponse>
</soapenv:Body>
</soapenv:Envelope>
