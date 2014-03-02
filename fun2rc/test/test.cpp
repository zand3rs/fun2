/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <string>

#include "fun2rc.hpp"
#include "libfuc.h"

/*============================================================================*/

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stdout, "Usage:\n%s <path-to-lib>\n", argv[0]);
        exit(1);
    }
    std::string libpath = argv[1];
    fprintf(stdout, "libpath=[%s]\n", libpath.c_str());

    void *handle = dlopen(libpath.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if (! handle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }

    // load the symbols
    const char *dlsym_error;

    Fun2RC_new* fun2rc_new = (Fun2RC_new*) dlsym(handle, "fun2rc_new");
    dlsym_error = dlerror();
    if (dlsym_error) {
        fprintf (stderr, "%s\n", dlsym_error);
        exit(1);
    }

    Fun2RC_del* fun2rc_del = (Fun2RC_del*) dlsym(handle, "fun2rc_del");
    dlsym_error = dlerror();
    if (dlsym_error) {
        fprintf (stderr, "%s\n", dlsym_error);
        exit(1);
    }

    Fun2RC* fun2rc = fun2rc_new();

    Fun2RC::param_t param;

    memset(&param, 0, sizeof(Fun2RC::param_t));
    snprintf(param.svc_url, sizeof(param.svc_url), "%s", "http://10.160.135.34/trigger/RbtTrigger.jsp");
    //snprintf(param.svc_url, sizeof(param.svc_url), "%s", "http://test.url.com");
    param.timeout=60;
    param.svc_port=23;
    //snprintf(param.svc_user, sizeof(param.svc_user), "%s", "TRIGGR");
    //snprintf(param.svc_pass, sizeof(param.svc_pass), "%s", "TRIGGER");
    //snprintf(param.svc_host, sizeof(param.svc_host), "%s", "192.50.100.41");
    snprintf(param.svc_user, sizeof(param.svc_user), "%s", "fun2");
    snprintf(param.svc_pass, sizeof(param.svc_pass), "%s", "fun2_t3$t!ng");
    snprintf(param.svc_host, sizeof(param.svc_host), "%s", "10.163.92.6");

    snprintf(param.db_host, sizeof(param.db_host), "%s", "trgprd");
    snprintf(param.db_user, sizeof(param.db_user), "%s", "trigger_owner");
    snprintf(param.db_pass, sizeof(param.db_pass), "%s", "trgown123");

    snprintf(param.svc_name, sizeof(param.svc_name), "%s", "CPS");
    snprintf(param.expiry, sizeof(param.expiry), "%s", "10.10.2009 01:00:00");
    LOG_INIT("/tmp/hlr.log", 5);
    param.logh = LOG_GET_HANDLE();


    param.timeout=60;
    param.svc_port=9882;

    fun2rc->initialize(&param);

#if 0
    //if (fun2rc->activate("639271010321") == 0) {
    /*if (fun2rc->getBalance("639271089049") == 0) {
      fprintf(stdout,"SUCCESSFULLY getBalance 639271089049.");
      } else {
      fprintf(stdout,"FAILED getBalance 639271089049.");
      }*/
    int outBalance = 0;
    outBalance = fun2rc->getBalance("639271089049");
    if (outBalance >= 0) {
        fprintf(stdout,"SUCCESSFULLY getBalance 639271089049 %d.\n",outBalance);
    } else {
        fprintf(stdout,"FAILED getBalance 639271089049.\n");
    }
#endif

    if (fun2rc->activate("639271010327") == 0) {
        fprintf(stdout,"SUCCESSFULLY activate 639271010327.\n");
    } else {
        fprintf(stdout,"FAILED activate 639271010327.\n");
    }
    if (fun2rc->deactivate("639271010327") == 0) {
        fprintf(stdout,"SUCCESSFULLY deactivate 639271010327.\n");
    } else {
        fprintf(stdout,"FAILED deactivate 639271010327.\n");
    }
    /*if (fun2rc->deactivate("639271010321") == 0) {
      fprintf(stdout,"SUCCESSFULLY Deactivated 639271010321.");
      } else {
      fprintf(stdout,"FAILED Deactivated 639271010321.");
      }*/

    fun2rc->deinitialize();

    fun2rc_del(fun2rc);

    dlclose(handle);
    LOG_DEINIT();
    return 0;
}

/******************************************************************************/
