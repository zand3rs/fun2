alter table request_log add cluster_node number default 1 not null;
create index req_log_node_idx on request_log (cluster_node, status);
alter table request_log add customer_type varchar2(16);
 
alter table response_log add cluster_node number default 1 not null;
create index res_log_node_idx on response_log (cluster_node, status);
alter table response_log add customer_type varchar2(16);

alter table manual_enrollment_log add cluster_node number default 1 not null;
alter table manual_enrollment_log add customer_type varchar2(16);
alter table manual_enrollment_log add min_bal NUMBER default -1; 

alter table sim_activation modify status null;

alter table request_log add request_origin varchar2(8) default 'SMS' not null; -- SMS or WEB
alter table request_log modify tran_type number(2);
alter table request_log add ref_id number;
alter table request_log modify msg null;

alter table keyword_msg drop constraint "KEYWORD_MSG_UK";
alter table keyword_msg add constraint "KEYWORD_MSG_UK" unique ("MESSAGE_ID");

alter table services add svc_url varchar2(256);
alter table services add svc_host varchar2(64);
alter table services add svc_port number(5);
alter table services add svc_user varchar2(32);
alter table services add svc_pass varchar2(32);
alter table services add db_host varchar2(64);
alter table services add db_user varchar2(32);
alter table services add db_pass varchar2(32);
alter table services add timeout_sec number(2);
alter table services add thread_count number(3);
alter table services add max_retry number(2);
alter table services add app_lib varchar2(32);


insert into services (status, priority, service_code, service_desc, svc_url, svc_host, svc_port, svc_user, svc_pass, db_host, db_user, db_pass, timeout_sec, thread_count, max_retry, app_lib)
    values (' ', 1, 'RBT', 'RBT', 'http://10.160.135.34/trigger/RbtTrigger.jsp', '', '', '', '', '', '', '', 60, 10, 3, 'librbt.so');


insert into services (status, priority, service_code, service_desc, svc_url, svc_host, svc_port, svc_user, svc_pass, db_host, db_user, db_pass, timeout_sec, thread_count, max_retry, app_lib)
    values (' ', 2, 'HLR', 'HLR', '', '192.50.100.41', '23', 'TRIGGR', 'TRIGGER', '', '', '', 60, 10, 3, 'libhlr.so');

