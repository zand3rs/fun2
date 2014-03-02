#ifndef ORADBDAILYBAL_HPP
#define ORADBDAILYBAL_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBDailyBal : public OraDB
{
    public:
        OraDBDailyBal(const char* ora_auth = "");
        virtual ~OraDBDailyBal();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getDailyBals(std::vector<daily_bal_t>* daily_bals, int cluster_node, int status, int limit=100);
        int updateDailyBal(daily_bal_t* daily_bal);
        int processDailyBal(daily_bal_t* daily_bal);

    private:
        int selectBind();
        int updateBind();
        int dailyBalBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_update;
        sqlo_stmt_handle_t _sth_pdb;

        int _var_retr;
        int _var_cluster_node;
        int _var_status;
        int _var_limit;

        daily_bal_t _daily_bal;

        short _ind_msisdn;
        short _ind_deact_dt;
        short _ind_daily_balance;
        short _ind_daily_ctr;
};

#endif
