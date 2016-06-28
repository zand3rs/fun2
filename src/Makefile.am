AM_CXXFLAGS = @CXXFLAGS@ @LIBSQLORA8_CFLAGS@ -I../ -I../fun2rc/include -I../libfuc/include -I../mxml
AM_CFLAGS = @CFLAGS@ @LIBSQLORA8_CFLAGS@ -I../ -I../fun2rc/include -I../libfuc/include -I../mxml

bin_PROGRAMS = fun2_request fun2_response fun2_notification fun2_transaction fun2_broadcast fun2_geoprobe fun2_ods fun2_default_unli fun2_mlp

fun2_request_SOURCES = \
                       data_struct.hpp \
                       fun2_request.cpp \
                       config.cpp config.hpp \
                       global.cpp global.hpp \
                       hlr.cpp hlr.hpp \
                       httpdb.cpp httpdb.hpp \
                       httpserver.cpp httpserver.hpp \
                       httputils.cpp httputils.hpp \
                       oradb.cpp oradb.hpp \
                       oradbrequest.cpp oradbrequest.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       request_handler.cpp request_handler.hpp \
                       signal_handler.cpp signal_handler.hpp \
                       sysmsg.cpp sysmsg.hpp

fun2_request_LDADD = \
             ../libfuc/lib/libfuc.a \
             -L/usr/local/lib -lpme -lssl -lpcre \
             @LIBSQLORA8_LIBS@

fun2_response_SOURCES = \
                       data_struct.hpp \
                       fun2_response.cpp \
                       config.cpp config.hpp \
                       global.cpp global.hpp \
                       base64.cpp base64.hpp \
                       httpclient.cpp httpclient.hpp \
                       httputils.cpp httputils.hpp \
                       oradb.cpp oradb.hpp \
                       oradbresponse.cpp oradbresponse.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       response_handler.cpp response_handler.hpp \
                       signal_handler.cpp signal_handler.hpp

fun2_response_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@ \
             @LIBCURL@

fun2_notification_SOURCES = \
                       data_struct.hpp \
                       fun2_notification.cpp \
                       config.cpp config.hpp \
                       global.cpp global.hpp \
                       base64.cpp base64.hpp \
                       httpclient.cpp httpclient.hpp \
                       httputils.cpp httputils.hpp \
                       oradb.cpp oradb.hpp \
                       oradbnotification.cpp oradbnotification.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       notification_handler.cpp notification_handler.hpp \
                       signal_handler.cpp signal_handler.hpp

fun2_notification_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@ \
             @LIBCURL@

fun2_transaction_SOURCES = \
                       data_struct.hpp \
                       fun2_transaction.cpp \
                       config.cpp config.hpp \
                       ccb_handler.cpp ccb_handler.hpp \
                       geoprobe_handler.cpp geoprobe_handler.hpp \
                       global.cpp global.hpp \
                       hlr.cpp hlr.hpp \
                       base64.cpp base64.hpp \
                       httpclient.cpp httpclient.hpp \
                       httputils.cpp httputils.hpp \
                       misc.cpp misc.hpp \
                       nsn.cpp nsn.hpp \
                       ods_handler.cpp ods_handler.hpp \
                       oradb.cpp oradb.hpp \
                       oradbccb.cpp oradbccb.hpp \
                       oradbgeoprobe.cpp oradbgeoprobe.hpp \
                       oradbods.cpp oradbods.hpp \
                       oradbrequest.cpp oradbrequest.hpp \
                       oradbresponse.cpp oradbresponse.hpp \
                       oradbnotification.cpp oradbnotification.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       transaction_handler.cpp transaction_handler.hpp \
                       signal_handler.cpp signal_handler.hpp \
                       sysmsg.cpp sysmsg.hpp

fun2_transaction_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@ \
             @LIBCURL@

fun2_broadcast_SOURCES = \
                       data_struct.hpp \
                       fun2_broadcast.cpp \
                       config.cpp config.hpp \
                       global.cpp global.hpp \
                       base64.cpp base64.hpp \
                       httpclient.cpp httpclient.hpp \
                       httputils.cpp httputils.hpp \
                       misc.cpp misc.hpp \
                       nsn.cpp nsn.hpp \
                       oradb.cpp oradb.hpp \
                       oradbdailybal.cpp oradbdailybal.hpp \
                       oradbbroadcast.cpp oradbbroadcast.hpp \
                       oradbresponse.cpp oradbresponse.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       broadcast_handler.cpp broadcast_handler.hpp \
                       signal_handler.cpp signal_handler.hpp \
                       sysmsg.cpp sysmsg.hpp

fun2_broadcast_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@ \
             @LIBCURL@

fun2_geoprobe_SOURCES = \
                       data_struct.hpp \
                       fun2_geoprobe.cpp \
                       ecsv.cpp ecsv.hpp \
                       oradb.cpp oradb.hpp \
                       oradbgeoprobe.cpp oradbgeoprobe.hpp

fun2_geoprobe_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@

fun2_ods_SOURCES = \
                       data_struct.hpp \
                       fun2_ods.cpp \
                       ecsv.cpp ecsv.hpp \
                       oradb.cpp oradb.hpp \
                       oradbods.cpp oradbods.hpp

fun2_ods_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@

fun2_default_unli_SOURCES = \
                       data_struct.hpp \
                       fun2_default_unli.cpp \
                       ecsv.cpp ecsv.hpp \
                       config.cpp config.hpp \
                       global.cpp global.hpp \
                       oradb.cpp oradb.hpp \
                       oradbdefaultunli.cpp oradbdefaultunli.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       default_unli_handler.cpp default_unli_handler.hpp \
                       signal_handler.cpp signal_handler.hpp

fun2_default_unli_LDADD = \
             ../libfuc/lib/libfuc.a \
             @LIBSQLORA8_LIBS@ \
             @LIBCURL@
             
fun2_mlp_SOURCES = \
                       data_struct.hpp \
                       fun2_mlp.cpp \
                       config.cpp config.hpp \
                       global.cpp global.hpp \
                       httpserver.cpp httpserver.hpp \
                       httputils.cpp httputils.hpp \
                       mlp_handler.cpp mlp_handler.hpp \
                       mlp_service.cpp mlp_service.hpp \
                       oradb.cpp oradb.hpp \
                       oradbrequest.cpp oradbrequest.hpp \
                       oradbsimple.cpp oradbsimple.hpp \
                       signal_handler.cpp signal_handler.hpp

fun2_mlp_LDADD = \
             ../mxml/libmxml.a \
             ../libfuc/lib/libfuc.a \
             -L/usr/local/lib -lpme -lssl -lpcre \
             @LIBSQLORA8_LIBS@


noinst_HEADERS =

