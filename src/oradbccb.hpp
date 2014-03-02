#ifndef ORADBCCB_HPP
#define ORADBCCB_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBCCB : public OraDB
{
    public:
        OraDBCCB(const char* ora_auth = "");
        virtual ~OraDBCCB();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getCCBs(std::vector<ccb_t>* ccbs, int cluster_node, int status, int limit=100);
        int insertCCB(ccb_t* ccb);
        int updateCCB(ccb_t* ccb);

    private:
        int selectBind();
        int insertBind();
        int updateBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_insert;
        sqlo_stmt_handle_t _sth_update;

        int _var_retr;
        int _var_cluster_node;
        int _var_status;
        int _var_limit;

        ccb_t _ccb;

        short _ind_msisdn;
        short _ind_ccb_action;
};

#endif
