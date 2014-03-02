#ifndef ORADBRESPONSE_HPP
#define ORADBRESPONSE_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBResponse : public OraDB
{
    public:
        OraDBResponse(const char* ora_auth = "");
        virtual ~OraDBResponse();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getResponses(std::vector<response_t>* responses, int cluster_node, int status, int limit=100);
        int insertResponse(const response_t* response);
        int updateResponse(const response_t* response);

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
        response_t _response;

        short _ind_a_no;
        short _ind_b_no;
        short _ind_msg;
        short _ind_txid;
};

#endif
