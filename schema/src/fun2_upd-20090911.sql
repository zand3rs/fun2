create table geoprobe_log
(
 msisdn varchar2(15) not null,
 info varchar2(60),
 dt_created date not null
 status number(4) default 0 not null;
);

create index geolog_dt_indx on geoprobe_log(dt_created);

alter table request_log add error_code number(4);

alter table response_log add tran_type number(2);
alter table response_log add msg_id varchar2(64);

alter table manual_enrollment_log add request_origin varchar2(8);
alter table customer_types add num_link number(4);
alter table customer_types add pre_act_day number(4);


alter table countries add COUNTRY_NAME_UPPER VARCHAR2(60);
 
CREATE OR REPLACE TRIGGER "COUNTRIES_INSERT_TRIGGER"
before insert on countries  
for each row
begin
   if :new.id is null or :new.id = 0
   then 
      select countries_seq.nextval into :new.id from dual;
   end if;
   :NEW.country_name_upper := upper(nvl(:NEW.country_name, :NEW.country_code));
end COUNTRIES_INSERT_TRIGGER;
/
CREATE OR REPLACE TRIGGER "COUNTRIES_UPDATE_TRIGGER"
before update of country_code, country_name on countries
for each row
begin
   :NEW.country_name_upper := upper(nvl(:NEW.country_name, :NEW.country_code));
end COUNTRIES_UPDATE_TRIGGER;
/
 
