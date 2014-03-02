#ifndef ORADBGEOPROBE_HPP
#define ORADBGEOPROBE_HPP

#include <vector>

#include "data_struct.hpp"
#include "oradb.hpp"

class OraDBGeoProbe : public OraDB
{
    public:
        OraDBGeoProbe(const char* ora_auth = "");
        virtual ~OraDBGeoProbe();
        virtual int initialize(const char* ora_auth = "");

    public:
        int getGeoProbes(std::vector<geo_probe_t>* geo_probes, int cluster_node, int status, int limit=100);
        int insertGeoProbe(geo_probe_t* geo_probe);
        int updateGeoProbe(geo_probe_t* geo_probe);
        int processGeoProbe(geo_probe_t* geo_probe);

    private:
        int selectBind();
        int insertBind();
        int updateBind();
        int geoProbeBind();

        sqlo_stmt_handle_t _sth_select;
        sqlo_stmt_handle_t _sth_insert;
        sqlo_stmt_handle_t _sth_update;
        sqlo_stmt_handle_t _sth_pgp;

        int _var_retr;
        int _var_cluster_node;
        int _var_status;
        int _var_limit;

        geo_probe_t _geo_probe;

        short _ind_msisdn;
        short _ind_country;
        short _ind_deactivation_date;
        short _ind_imsi;
        short _ind_sccp;
};

#endif
