insert into keyword_msg
    (id, keyword, sub_keyword, description, customer_type, message_type, status, dt_created, message_id, msg_1)
    values
    (keyword_msg_seq.nextval, 'GROAM', 'NO', 'GROAM NO pre-deactivation', 'TRIGGER', 'ALERT', 'ACTIVE', sysdate,
     'GROAM_NO_PRE_DEACT',
     'Your roaming has now been deactivated. To activate your roaming, you may call our Globe Roaming Support Hotline at +632 7301212 from any phone in the country you are currently in.'
    );


insert into keyword_msg
    (id, keyword, sub_keyword, description, customer_type, message_type, status, dt_created, message_id, msg_1)
    values
    (keyword_msg_seq.nextval, 'GROAM', 'NO', 'GROAM NO successful deactivation', 'TRIGGER', 'ALERT', 'INACTIVE', sysdate,
     'GROAM_NO_SUCCESSFUL',
     'Your roaming has now been deactivated. To activate your roaming, you may call our Globe Roaming Support Hotline at +632 7301212 from any phone in the country you are currently in.'
     );

