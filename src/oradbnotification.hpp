#ifndef ORADBNOTIFICATION_HPP
#define ORADBNOTIFICATION_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBNotification : public OraDB
{
    public:
        OraDBNotification(const char* ora_auth = "");
        virtual ~OraDBNotification();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getNotifications(std::vector<notification_t>* notifications, int cluster_node, int status, int limit=100);
        int insertNotification(const notification_t* notification);
        int updateNotification(const notification_t* notification);

    private:
        int selectBind();
        int insertBind();
        int updateBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_insert;
        sqlo_stmt_handle_t _sth_update;

        int _var_id;
        int _var_cluster_node;
        int _var_status;
        int _var_limit;
        notification_t _notification;

        short _ind_msisdn;
};

#endif
