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
    snprintf(param.svc_url, sizeof(param.svc_url), "%s", "http://localhost/zander/soap.php");
    param.timeout=60;
    LOG_INIT("/tmp/sset.log", 5);
    param.logh = LOG_GET_HANDLE();


    fun2rc->initialize(&param);

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

    fun2rc->deinitialize();

    fun2rc_del(fun2rc);

    dlclose(handle);
    LOG_DEINIT();
    return 0;
}

/******************************************************************************/
