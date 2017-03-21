alter table services add BRAND VARCHAR2(16) default 'PREPAID';
alter table request_log add BRAND VARCHAR2(16) default 'PREPAID';
alter table broadcast_log add BRAND VARCHAR2(16) default 'PREPAID';
alter table conditioner_log add BRAND VARCHAR2(16) default 'PREPAID';
alter table keyword_msg add BRAND VARCHAR2(16) default 'PREPAID';
