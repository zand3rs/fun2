#ifndef ORADBODS_HPP
#define ORADBODS_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBODS : public OraDB
{
    public:
        OraDBODS(const char* ora_auth = "");
        virtual ~OraDBODS();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getODSs(std::vector<ods_t>* odss, int cluster_node, int status, int limit=100);
        int insertODS(ods_t* ods);
        int updateODS(ods_t* ods);
        int processODS(ods_t* ods);

    private:
        int selectBind();
        int insertBind();
        int updateBind();
        int odsBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_insert;
        sqlo_stmt_handle_t _sth_update;
        sqlo_stmt_handle_t _sth_po;

        int _var_retr;
        int _var_cluster_node;
        int _var_status;
        int _var_limit;

        ods_t _ods;

        short _ind_msisdn;
};

#endif
