#ifndef ORADBDEFAULTUNLI_HPP
#define ORADBDEFAULTUNLI_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBDefaultUnli : public OraDB
{
    public:
        OraDBDefaultUnli(const char* ora_auth = "");
        virtual ~OraDBDefaultUnli();
        virtual int initialize(const char* ora_auth = "");

    public:
        int processDefaultUnli(default_unli_t* default_unli);
        int getLastFileProcessed(default_unli_t* default_unli);

    private:
        int defaultUnliBind();
        int lastFileProcessedBind();

        sqlo_stmt_handle_t _sth_pdu;
        sqlo_stmt_handle_t _sth_lfp;

        int _var_retr;
        int _var_cluster_node;

        default_unli_t _default_unli;
};

#endif
