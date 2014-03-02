alter table sim_activation drop column BALANCE_THRESHOLD;
alter table sim_activation add daily_bal number(8,2);
alter table sim_activation add min_bal number(8,2);
alter table sim_activation add duration number(3);
alter table sim_activation add extended number(1) default 0;
alter table manual_enrollment_log add breathing_period number(4);
alter table manual_enrollment_log add link_to number(12);
alter table request_log add min_bal number(4) default 0 not null;
alter table services add SVC_NAME VARCHAR2(64);
alter table services add expiry VARCHAR2(64);
alter table customer_types add breathing_period NUMBER(3);
alter table manual_enrollment_log modify min_bal default 0;

alter table response_log modify msg varchar2(1024);

