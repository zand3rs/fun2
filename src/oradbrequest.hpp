#ifndef ORADBREQUEST_HPP
#define ORADBREQUEST_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBRequest : public OraDB
{
    public:
        OraDBRequest(const char* ora_auth = "");
        virtual ~OraDBRequest();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getRequests(std::vector<request_t>* requests, int cluster_node, int step_no, int status, int limit=100);
        int insertRequest(const request_t* request);
        int updateRequest(const request_t* request);

        int getManualRequests(std::vector<request_t>* requests, int cluster_node, int status, int limit=100);
        int updateManualRequest(const request_t* request);

        int initTran(request_t* request);
        int processTran(request_t* request);
        int usurfActivation(request_t* request);
        int usurfDeactivation(request_t* request);

        int insertKeywordRequest(const request_t* request);

        int processMlp(request_t* request);
        int processShampoo(request_t* request);

    private:
        int _do_bind();

        int selectBind();
        int insertBind();
        int updateBind();

        int selectManualBind();
        int updateManualBind();

        int initTranBind();
        int processTranBind();
        int usurfActivationBind();
        int usurfDeactivationBind();

        int insertKeywordBind();
        int processMlpBind();
        int processShampooBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_insert;
        sqlo_stmt_handle_t _sth_update;

        sqlo_stmt_handle_t _sth_select2;
        sqlo_stmt_handle_t _sth_update2;

        sqlo_stmt_handle_t _sth_it;
        sqlo_stmt_handle_t _sth_pt;
        sqlo_stmt_handle_t _sth_ua;
        sqlo_stmt_handle_t _sth_ud;

        sqlo_stmt_handle_t _sth_insert_kw;
        sqlo_stmt_handle_t _sth_process_mlp;
        sqlo_stmt_handle_t _sth_process_shampoo;

        int _var_id;
        int _var_cluster_node;
        int _var_step_no;
        int _var_last_step_no;
        int _var_status;
        int _var_limit;

        int _var_retr;
        int _var_trantype;
        int _var_req_id;
        int _var_ref_id;

        int _var_duration;

        char _var_msisdn[16];
        char _var_country[32];
        char _var_service_id[32];

        char _var_extra_i_1[320];
        char _var_extra_i_2[320];
        char _var_extra_i_3[320];
        char _var_extra_i_4[320];

        char _var_extra_o_1[320];
        char _var_extra_o_2[320];
        char _var_extra_o_3[320];

        request_t _request;

        short _ind_a_no;
        short _ind_b_no;
        short _ind_msg;
        short _ind_customer_type;
        short _ind_request_origin;
        short _ind_imsi;
        short _ind_activation_date;
        short _ind_deactivation_date;
        short _ind_duration;
        short _ind_gsm_num;
        short _ind_result_code;
        short _ind_silent;
        short _ind_nsn_flag;

        short _ind_extra_1;
        short _ind_extra_2;
        short _ind_extra_3;
};

#endif
