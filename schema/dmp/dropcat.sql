set pages 0
set lines 256
set feedback off
col sql_text form a128
spoo dropcat.lst
select 'drop ' || table_type || ' ' || table_name || decode(table_type, 'TABLE', ' cascade constraints;', ';') sql_text
from cat
/
spoo off
set feedback on
set pages 128
@dropcat.lst
