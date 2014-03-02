create table ARDSRequest(
        id number NOT NULL,
        imsi varchar2(20) NOT NULL,
        enrollment_type varchar2(12) NOT NULL,
        status  number(4) default 0 NOT NULL,
        rna  number(1) default 0 NOT NULL,
        start_tm     date NOT NULL,
        end_tm       date,
        constraint ARDSRequest_pk primary key (id)
        );
create sequence ARDSRequest_id_seq start with 623482 increment by 1 order nocache nocycle;
create index ARDSRequest_start_tm_idx on ARDSRequest (start_tm);
create index ARDSRequest_end_tm_idx on ARDSRequest (end_tm);
create index ARDSRequest_status_idx on ARDSRequest(id,status);

create table ARDSResponse(
        id number NOT NULL,
        imsi varchar2(20) NOT NULL,
        enrollment_type varchar2(12) NOT NULL,
        status  number(4) default 0 NOT NULL,
        cause  number(1) default 0 NOT NULL,
        start_tm     date NOT NULL,
        end_tm       date,
        constraint ARDSResponse_pk primary key (id)
        );
create sequence ARDSResponse_id_seq start with 623482 increment by 1 order nocache nocycle;
create index ARDSResponse_start_tm_idx on ARDSResponse (start_tm);
create index ARDSResponse_end_tm_idx on ARDSResponse (end_tm);
create index ARDSResponse_status_idx on ARDSResponse(id,status);

create table keywordRequest(
        id number NOT NULL,
        ref_id number NOT NULL,
        msisdn varchar(20) NOT NULL,
        imsi varchar2(20) NOT NULL,
        enrollment_type varchar2(12) NOT NULL,
        status  number(4) default 0 NOT NULL,
        start_tm     date NOT NULL,
        end_tm       date,
        constraint keywordRequest_pk primary key (id)
        );
create sequence keywordRequest_id_seq start with 623482 increment by 1 order nocache nocycle;
create index keywordRequest_start_tm_idx on keywordRequest (start_tm);
create index keywordRequest_end_tm_idx on keywordRequest (end_tm);
create index keywordRequest_status_idx on keywordRequest(msisdn,imsi,status);

alter table manual_enrollment_log add imsi varchar2(30);

