/******************************************************************************/

#ifndef _FUN2RC_HPP_
#define _FUN2RC_HPP_


/*============================================================================*/

class Fun2RC {

    public:
        typedef struct param_t {
            char svc_url[1024];
            char svc_name[256];
            char svc_host[64];
            int  svc_port;
            char svc_user[32];
            char svc_pass[32];
            char svc_key[256];
            char svc_cert[256];
            char svc_cacert[256];
            char db_host[32];
            char db_user[32];
            char db_pass[32];
            char expiry[32];
            int timeout;
            void* logh;
        } param_t;

        Fun2RC(const param_t* param = NULL) {
            memset(&_param, 0, sizeof(param_t));
            if (param) {
                memcpy(&_param, param, sizeof(param_t));
            }
        }
        virtual ~Fun2RC() {}

        int initialize(const param_t* param) {
            if (! param) {
                return -1;
            }
            memcpy(&_param, param, sizeof(param_t));
            return initialize();
        }

        //-- pure virtual functions...
        virtual char* const moduleName() const = 0;

        virtual int initialize() = 0;
        virtual int deinitialize() = 0;

        virtual int activate(const char *msisdn) = 0;
        virtual int deactivate(const char *msisdn) = 0;

        virtual int getBalance(const char *msisdn) { return 0; }
        virtual int getIMSI(const char *msisdn, char *imsi, int imsi_size) { return 0; }

    protected:
        param_t _param;

};

/*============================================================================*/
// types of the class factories

typedef Fun2RC* Fun2RC_new();
typedef void Fun2RC_del(Fun2RC*);

#endif

/******************************************************************************/
