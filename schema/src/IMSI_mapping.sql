create table HLR_IMSI_MAPPING (
   server_ip varchar2(14) not null,
   imsi varchar2(15) not null,
   status varchar2(12) default 'ACTIVE' not null,
   server_name varchar2(30) not null,
   created_by varchar2(30) not null,
   dt_created date not null,
   constraint imsi_pk primary key (imsi, server_ip)
);

select s.SERVER_IP, s.SERVER_PORT, s.SERVER_NAME, s.USERNAME, s.PASSWORD, s.TIMEOUT, s.RETRY_CNT, m.IMSI
from   HLR_SERVERS s, HLR_IMSI_MAPPING m
where  s.SERVER_IP = m.SERVER_IP 
and    (m.IMSI = substr('515021808853427', 1, 7)) 

# select c.IRSSET, c.NORMALSSET 
# from   SSET_MAPPING c 
# and    c.MSISDN_FR<='?' and c.MSISDN_TO>='?'


insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.13.41', '5150202', 'HLR LAHUG', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.13.41', '5150233', 'HLR LAHUG', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.13.41', '5150250', 'HLR LAHUG', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.40', '5150207', 'HLR MARIKINA4i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.40', '5150238', 'HLR MARIKINA4i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.100.30', '5150201', 'HLR PIONEER1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.100.30', '5150232', 'HLR PIONEER1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.100.31', '5150209', 'HLR PIONEER2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.100.31', '5150212', 'HLR PIONEER2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.100.32', '5150239', 'HLR PIONEER3i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.100.32', '5150210', 'HLR PIONEER3i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.99.89.60', '5150211', 'HLR VCRUZ1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.99.89.60', '5150237', 'HLR VCRUZ1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.99.89.61', '5150213', 'HLR VCRUZ2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.102.42', '5150215', 'HLR CANERO1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.102.42', '5150227', 'HLR CANERO1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.102.42', '5150229', 'HLR CANERO1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.161.16.42', '5150218', 'HLR CANERO2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.161.16.42', '5150205', 'HLR CANERO2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.161.16.42', '5150204', 'HLR CANERO2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.161.16.42', '5150228', 'HLR CANERO2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.101.41', '5150216', 'HLR CARMONA1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.23.101.41', '5150230', 'HLR CARMONA1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.163.16.42', '5150217', 'HLR CARMONA2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.163.16.42', '5150231', 'HLR CARMONA2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.163.16.42', '5150234', 'HLR CARMONA2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.163.16.42', '5150243', 'HLR CARMONA2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.51', '5150219', 'HLR VALERO 1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.51', '5150208', 'HLR VALERO 1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.51', '5150203', 'HLR VALERO 1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.51', '5150206', 'HLR VALERO 1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.51', '5150214', 'HLR VALERO 1i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.52', '5150221', 'HLR VALERO 2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.52', '5150224', 'HLR VALERO 2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.52', '5150242', 'HLR VALERO 2i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.53', '5150222', 'HLR VALERO 3i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.53', '5150225', 'HLR VALERO 3i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.53', '5150236', 'HLR VALERO 3i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('10.162.16.53', '5150241', 'HLR VALERO 3i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150223', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150226', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150235', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150240', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150260', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150261', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150262', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.9.163.89', '5150270', 'HLR MARIKINA 5i', user, sysdate );
insert into hlr_imsi_mapping (server_ip, imsi, server_name, created_by, dt_created) values ('192.50.100.41', '5150299', 'HLR TESTBED i', user, sysdate ); 
commit;
