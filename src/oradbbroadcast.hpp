#ifndef ORADBBROADCAST_HPP
#define ORADBBROADCAST_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBBroadcast : public OraDB
{
    public:
        OraDBBroadcast(const char* ora_auth = "");
        virtual ~OraDBBroadcast();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getBroadcasts(std::vector<broadcast_t>* broadcasts, const char* brand, int cluster_node, int status, int limit=100);
        int updateBroadcast(broadcast_t* broadcast);

    private:
        int selectBind();
        int updateBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_update;

        char _var_brand[16];

        int _var_retr;
        int _var_cluster_node;
        int _var_status;
        int _var_limit;

        broadcast_t _broadcast;

        short _ind_msisdn;
        short _ind_msg_id;
        short _ind_msg;
};

#endif
