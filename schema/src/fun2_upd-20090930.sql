alter table request_log add result_code number(3) DEFAULT 0 NOT NULL;

alter table in_servers add svc_name varchar2(64);
alter table in_servers add expiry varchar2(64);

update in_servers set username='fun2', password='fun2_t3$t!ng', timeout=60, svc_name='CPS', expiry='10.10.2099 01:00';
commit;

update hlr_servers set username='TRIGGR', password='TRIGGER', timeout=60;
commit;
