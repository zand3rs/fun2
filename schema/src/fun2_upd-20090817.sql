PROMPT TABLE: keywords

create table keywords (
  id             number NOT NULL,
  keyword        varchar2(30) NOT NULL,
  sub_keyword    varchar2(30),
  keyword_desc   varchar2(60) NOT NULL,
  status         varchar2(12) NOT NULL,  -- ACTIVE or INACTIVE
  created_by     varchar2(30),
  dt_created     date,
  updated_by     varchar2(30),
  dt_updated     date,
  constraint keywords_pk primary key (id),
  constraint keywords_uk unique (keyword, sub_keyword)
);

PROMPT TABLE: keyword_msg

create table keyword_msg (
  id             number NOT NULL,
  keyword        varchar2(30) NOT NULL,
  sub_keyword    varchar2(30),
  description    varchar2(60) NOT NULL,
  customer_type  varchar2(16) DEFAULT 'TRIGGER' NOT NULL, -- other values can be FUN etc...
  message_type   char(1) DEFAULT 'A' not null, -- A-Alert / E-Error
  message_key    varchar2(30) NOT NULL, -- <unique string>
  msg_1          varchar2(255) NOT NULL,
  msg_2          varchar2(255),
  msg_3          varchar2(255),
  status         varchar2(12) NOT NULL,  -- ACTIVE or INACTIVE
  created_by     varchar2(30),
  dt_created     date,
  updated_by     varchar2(30),
  dt_updated     date,
  constraint keyword_msg_pk primary key (id),
  constraint keyword_msg_uk unique (keyword, sub_keyword, message_type)
);


-------------------------------------------------------------------------

insert into keyword_msg values(1,  'GROAM'  , 'ACTIVATE'  , 'description', 'FUN2', 'A', '', 'GROAM   ACTIVATE   message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(2,  'GROAM'  , 'DEACTIVATE', 'description', 'FUN2', 'A', '', 'GROAM   DEACTIVATE message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(3,  'GEXT'   , 'EXTEND'    , 'description', 'FUN2', 'A', '', 'GEXT    EXTEND     message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(4,  'GLIST'  , 'GLIST'     , 'description', 'FUN2', 'A', '', 'GLIST   GLIST      message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(5,  'GROAM'  , 'GROAM'     , 'description', 'FUN2', 'A', '', 'GROAM   GROAM      message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(6,  'GHELP'  , 'HELP'      , 'description', 'FUN2', 'A', '', 'GHELP   HELP       message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(7,  'GLIST'  , 'LIST'      , 'description', 'FUN2', 'A', '', 'GLIST   LIST       message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(8,  'GEXT'   , 'MEXTEND'   , 'description', 'FUN2', 'A', '', 'GEXT    MEXTEND    message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(9,  'GROAM'  , 'OFF'       , 'description', 'FUN2', 'A', '', 'GROAM   OFF        message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(10, 'GROAM'  , 'ON'        , 'description', 'FUN2', 'A', '', 'GROAM   ON         message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(11, 'GROAM'  , 'OVERACT'   , 'description', 'FUN2', 'A', '', 'GROAM   OVERACT    message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(12, 'GEXT'   , 'OVEREXT'   , 'description', 'FUN2', 'A', '', 'GEXT    OVEREXT    message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);
insert into keyword_msg values(13, 'GSTATUS', 'STATUS'    , 'description', 'FUN2', 'A', '', 'GSTATUS STATUS     message 1', '', '', 'ACTIVE', '', sysdate, '', sysdate);

-------------------------------------------------------------------------

