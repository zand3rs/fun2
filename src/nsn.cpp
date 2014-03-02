/******************************************************************************/
#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <errno.h>
#include <fstream>
#include <algorithm>
#include <ctype.h>
#include <netdb.h>
#include <pwd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "libfuc.h"
#include "oradbsimple.hpp"
#include "nsn.hpp"
/*============================================================================*/
int NSN::_initialize()
{
    buffer = (char*)malloc(buffer_size);
    if (! buffer) {
        return -1;
    }
    memset(buffer, 0, buffer_size);

    LOG_INFO("%s::%s: ...", __class__, __func__);

    return 0;
}
/*----------------------------------------------------------------------------*/

int NSN::_deinitialize()
{
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }

    LOG_INFO("%s::%s: ...", __class__, __func__);

    return 0;
}

/*----------------------------------------------------------------------------*/
char *NSN::getPingUrl()
{
    int nbytes;
    char *pbuf = NULL;
    char *testPointer = NULL; 

    memset(session_url, 0, sizeof(session_url));
    if (connectSocket() != 0) {
        LOG_ERROR("%s::%s: Connection failed: host: %s, port: %d", __class__, __func__, host, port);
        return NULL;
    }

    sprintf(buffer,"GET / HTTP/1.1\r\nAccept: text/plain\r\nSMAF-Action: ping\r\nSMAF-ServiceName: %s\r\nHost: %s:%d\r\n\r\n",nsnUsername,host,port);
    //fprintf(stdout,"GetPingUrl data being sent:%s",buffer);
    if (sendData(buffer) == 0) {
        nbytes = readStream(buffer, buffer_size);	
        if (nbytes <= 0) {
            return NULL;
        }
        if (strstr(buffer,"Location")) {
            //fprintf(stdout,"This is the stream:%s\n",buffer);
            pbuf = strtok_r(buffer,"\r",&testPointer);
            pbuf = strtok_r(NULL,": ",&testPointer);
            pbuf = strtok_r(NULL,"\r ",&testPointer);
            if (pbuf) {
                snprintf(session_url, sizeof(session_url), "%s", pbuf);
                //fprintf(stdout,"URL:%s\n",session_url);
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        //fprintf(stdout,"Unable to Establish connection to NSN IP:%s and NSN Port: %d",host,port);
        return NULL;
    }
    return session_url;
}
/*----------------------------------------------------------------------------*/
int NSN::ping()
{
    int nbytes;

    sprintf(buffer,"GET %s HTTP/1.1\r\nAccept: text/plain\r\nSMAF-Action: ping\r\nSMAF-ServiceName: %s\r\nUser-Agent: Jakarta Commons-HttpClient/2.0.1\r\nHost: %s:%d\r\n\r\n",session_url,nsnUsername,host,port);
    //fprintf(stdout,"Ping data being sent:\n%s",buffer);
    if (sendData(buffer) == 0) {
        nbytes = readStream(buffer, buffer_size);	
        if (nbytes <= 0) {
            return -1;
        }
        //fprintf(stdout,"This is buffer:%s",buffer);
        if (strstr(buffer,"OK")) {
            //fprintf(stdout,"Ping Successful\n");
            return 0;
        } else {
            //fprintf(stdout,"Ping Failed\n");
            return -1;
        }
    } else {
        //fprintf(stdout,"Unable to Establish connection to NSN IP:%s and NSN Port: %d",host,port);
        return -1;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
int NSN::login()
{
    int nbytes;

    sprintf(buffer,"GET %s HTTP/1.1\r\nAccept: text/plain\r\nContent-type: text/plain; charset=UTF-8\r\nSMAF-Action: login\r\nSMAF-UserName: %s\r\nSMAF-Password: %s\r\nSMAF-ServiceName: %s\r\nUser-Agent: Jakarta Commons-HttpClient/2.0.1\r\nHost: %s:%d\r\nContent-Length: 0\r\n\r\n",session_url,username,password,nsnUsername,host,port);
    //fprintf(stdout,"Login data being sent:\n%s",buffer);
    if (sendData(buffer) == 0) {
        nbytes = readStream(buffer, buffer_size);	
        if (nbytes <= 0) {
            return -1;
        }
        //fprintf(stdout,"This is buffer:%s",buffer);
        if (strstr(buffer,"SMAF-ErrorText: OK")) {
            return 0;
        } else {
            return -1;
        }
    } else {
        //fprintf(stdout,"Unable to Establish connection to NSN IP:%s and NSN Port: %d",host,port);
        return -1;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
int NSN::logout()
{
    int nbytes;

    sprintf(buffer,"GET %s HTTP/1.1\r\nAccept: text/plain\r\nSMAF-Action: logout\r\nSMAF-ServiceName: %s\r\nUser-Agent: Jakarta Commons-HttpClient/2.0.1\r\nHost: %s:%d\r\n\r\n",session_url,nsnUsername,host,port);
    //fprintf(stdout,"Logout data being sent:\n%s",buffer);
    if (sendData(buffer) == 0) {
        nbytes = readStream(buffer, buffer_size);	
        if (nbytes <= 0) {
            return -1;
        }
        //fprintf(stdout,"This is buffer:%s",buffer);
        if (strstr(buffer,"OK")) {
            return 0;
        } else {
            return -1;
        }
    } else {
        //fprintf(stdout,"Unable to Establish connection to NSN IP:%s and NSN Port: %d",host,port);
        return -1;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
int NSN::executeGetBalance(char *msisdn)
{
    int nbytes;
    char *pbuf = NULL;
    char *testPointer = NULL; 

    snprintf(payload,sizeof(payload),"Op=GetCustomerDataSubscriber|CUSTOMERID|Subscriber__DAS.OnPeakAccountID.Balance|Subscriber__DAS.FreeSmsAccount.Balance\n1-1-%s",msisdn);
    sprintf(buffer,"POST %s HTTP/1.1\r\nAccept: text/plain\r\nContent-type: text/plain; charset=ISO-8859-1\r\nSMAF-ServiceName: %s\r\nSMAF-Action: execute\r\nUser-Agent: Jakarta Commons-HttpClient/2.0.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",session_url,nsnUsername,host,port,(int)strlen(payload)+1,payload);

    LOG_DEBUG("%s::%s: Get balance command: %s", __class__, __func__, buffer);

    //fprintf(stdout,"GetBalance data being sent:\n%s",buffer);
    if (sendData(buffer) == 0) {
        nbytes = readStream(buffer, buffer_size);	
        if (nbytes <= 0) {
            return -1;
        }
        //fprintf(stdout,"GetBalance buffer:%s",buffer);
        if (strstr(buffer,msisdn)) {
            pbuf = strtok_r(buffer,"|",&testPointer);
            pbuf = strtok_r(NULL,"|",&testPointer);
            pbuf = strtok_r(NULL,"|",&testPointer);
            pbuf = strtok_r(NULL,"|",&testPointer);
            pbuf = strtok_r(NULL,"|",&testPointer);
            if (pbuf) {
                //fprintf(stdout,"Balance:%s\n",pbuf);
                return (int)strtol(pbuf, NULL, 10);
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    } else {
        //fprintf(stdout,"Unable to Establish connection to NSN IP:%s and NSN Port: %d",host,port);
        return -1;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
int NSN::connectSocket() 
{
    int herr;

    m_serverSocket = socket(AF_INET,SOCK_STREAM,0);
    if (m_serverSocket != -1) {
        //m_pHost=gethostbyname(host);
        gethostbyname_r(host, &m_Host, buffer, buffer_size, &m_pHost, &herr);

        serverSocket.sin_addr = *((struct in_addr *)m_pHost->h_addr);
        serverSocket.sin_family = AF_INET;
        serverSocket.sin_port = htons(port);
        memset(&(serverSocket.sin_zero), '\0', 8);
        if(connect(m_serverSocket, (struct sockaddr *)&serverSocket, sizeof(struct sockaddr)) != -1){
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}
/*----------------------------------------------------------------------------*/
int NSN::readStream(char *buf, int buf_sz) 
{
    memset(buf, 0, buf_sz);
    int received = recv(m_serverSocket, buf, buf_sz-1, 0);
    return received;
}
/*----------------------------------------------------------------------------*/
int NSN::sendData(char *data) 
{
    struct pollfd pfd[1];
    pfd[0].fd = m_serverSocket;
    pfd[0].events = POLLOUT;
    int nRet = poll (pfd,1,timeout);
    if (nRet > 0) {
        if (pfd[0].revents & POLLERR) {
            return -1;
        } else if (pfd[0].revents & POLLOUT) {
            if (send(m_serverSocket,data,strlen(data),0) == -1) {
                return -1;
            } else {
                return 0;
            }
        }
    } else {
        return -1;
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
#ifdef __DEVEL__
int NSN::getBalance(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);
    return 10000;
}
#else
int NSN::getBalance(const char *msisdn)
{
    LOG_INFO("%s::%s: msisdn: %s", __class__, __func__, msisdn);

    if (OraDBSimple::init_lib() < 0) {
        LOG_CRITICAL("%s::%s: Unable to initialize OCI!!!", __class__, __func__);
        return -1;
    }

    OraDBSimple conn;
    if (conn.initialize(_ora_auth.c_str()) < 0) {
        LOG_ERROR("%s::%s: Unable to connect to oracle: ora_auth: %s", __class__, __func__, _ora_auth.c_str());
        return -1;
    }

    OraDBSimple::stmt_handle_t res = OraDBSimple::STH_INIT;
    const char** row;

    char q[512];
    snprintf(q, sizeof(q), "select a.SERVER_NAME, a.SERVER_IP, a.SERVER_PORT, a.USERNAME, a.PASSWORD"
            ", a.TIMEOUT, a.RETRY_CNT, a.SVC_NAME, a.EXPIRY, b.MSISDN_FR, b.MSISDN_TO"
            " from in_servers a, in_server_mapping b"
            " where a.SERVER_NAME = b.SERVER_NAME"
            " AND %s between b.MSISDN_FR and b.MSISDN_TO", msisdn);

    //-- initialize parameters...
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));
    memset(host, 0, sizeof(host));
    memset(nsnUsername, 0, sizeof(nsnUsername));
    memset(nsnExpirationDate, 0, sizeof(nsnExpirationDate));
    timeout = 60000;
    port = 0;

    conn.query(&res, q);
    if ((row = conn.fetchRow(res)) != NULL) {
        snprintf(host, sizeof(host), "%s", row[1]);
        port = strtol(row[2], NULL, 10);

        snprintf(username, sizeof(username), "%s", row[3]);
        snprintf(password, sizeof(password), "%s", row[4]);
        timeout = (*row[5]) ? strtol(row[5], NULL, 10) * 1000 : 60000;

        snprintf(nsnUsername, sizeof(nsnUsername), "%s", row[7]);
        snprintf(nsnExpirationDate, sizeof(nsnExpirationDate), "%s", (*row[8]) ? row[8] : "10.10.2099 01:00");
    }
    conn.freeResult(res);
    conn.destroy_db();

    if (! *nsnUsername || ! *username || ! *host || ! *password || ! port) {
        LOG_ERROR("%s::%s: Missing required parameters: host: %s, port: %d, user: %s, pass: %s, nsn_name: %s", __class__, __func__,
                host, port, username, password, nsnUsername);
        LOG_DEBUG("%s::%s: Database query: %s", __class__, __func__, q);
        return -1;
    }

    if (! getPingUrl()) {
        LOG_ERROR("%s::%s: Failed: getPingUrl", __class__, __func__);
        close(m_serverSocket);
        shutdown(m_serverSocket,SHUT_RDWR);
        return -1;
    }

    if (ping() != 0) {
        LOG_ERROR("%s::%s: Failed: ping", __class__, __func__);
        close(m_serverSocket);
        shutdown(m_serverSocket,SHUT_RDWR);
        return -1;
    }

    if (login() != 0) {
        LOG_ERROR("%s::%s: Failed: login", __class__, __func__);
        close(m_serverSocket);
        shutdown(m_serverSocket,SHUT_RDWR);
        return -1;
    }

    int balance = executeGetBalance((char *)msisdn);

    logout();
    close(m_serverSocket);
    shutdown(m_serverSocket,SHUT_RDWR);

    return balance;
}
#endif
/******************************************************************************/
