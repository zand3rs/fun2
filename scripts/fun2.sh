#!/bin/bash

BASE_DIR="${0%/*}"
APP_DIR_PATH=". ../ ../app"
CFG_DIR_PATH="cfg ../cfg"

APP_BIN_DIR=""
APP_CFG_DIR="cfg"

APP_NAME="fun2"
REQUEST_BIN="fun2_request"
RESPONSE_BIN="fun2_response"
NOTIFICATION_BIN="fun2_notification"
TRANSACTION_BIN="fun2_transaction"
BROADCAST_BIN="fun2_broadcast"
DEFAULT_UNLI_BIN="fun2_default_unli"
MLP_BIN="fun2_mlp"
SHAMPOO_BIN="fun2_shampoo"
APP_BIN_LIST="$REQUEST_BIN $RESPONSE_BIN $NOTIFICATION_BIN $TRANSACTION_BIN $BROADCAST_BIN $DEFAULT_UNLI_BIN $MLP_BIN $SHAMPOO_BIN"

##---------------------------------------------------------------------------

for CFG_DIR in $CFG_DIR_PATH; do
    if [ -d "$BASE_DIR/$CFG_DIR" ]; then
        APP_CFG_DIR="$BASE_DIR/$CFG_DIR"
        break
    fi
done

for APP_DIR in $APP_DIR_PATH; do
    if [ -d "$BASE_DIR/$APP_DIR" ]; then
        for APP_BIN in $APP_BIN_LIST; do
            if [ -f "$BASE_DIR/$APP_DIR/$APP_BIN" ]; then
                APP_BIN_DIR="$BASE_DIR/$APP_DIR"
                break
            fi
        done
    fi
    if [ ! -z $APP_BIN_DIR ]; then
        break
    fi
done
if [ -z $APP_BIN_DIR ]; then
    echo "Unable to locate application directory!"
    exit 1
fi

##---------------------------------------------------------------------------

case $1 in
    start)
        echo "Starting $APP_NAME..."

        if [ -d $APP_BIN_DIR ]; then
            cd $APP_BIN_DIR
        else
            echo "Directory \"$APP_BIN_DIR\" does not exists!"
            exit 1
        fi

        if [ ! -d $APP_CFG_DIR ]; then
            echo "Config directory \"$APP_CFG_DIR\" does not exists!"
            exit 1
        fi

        for APP_BIN in $APP_BIN_LIST; do
            ./$APP_BIN -c $APP_CFG_DIR/$APP_BIN.cfg >/dev/null 2>&1
            pid=`pgrep -f "$APP_BIN "`
            if [ ! -z "$pid" ]; then
                echo "$APP_BIN started"
            fi
        done
        echo "Done."
        ;;
    stop)
        echo "Stopping $APP_NAME..."
        for APP_BIN in $APP_BIN_LIST; do
            pid=`pgrep -f "$APP_BIN "`
            if [ ! -z "$pid" ]; then
                kill $pid
                for i in {1..5}; do
                    pid=`pgrep -f "$APP_BIN "`
                    if [ -z "$pid" ]; then
                        break
                    fi
                    sleep 1
                done
            fi

            pid=`pgrep -f "$APP_BIN "`
            if [ -z "$pid" ]; then
                echo "$APP_BIN stopped"
            else
                echo "$APP_BIN is still alive, try: $0 kill"
            fi
        done
        echo "Done."
        ;;
    restart)
        $0 stop
        $0 start
        $0 status
        ;;
    kill)
        echo "Killing $APP_NAME..."
        for APP_BIN in $APP_BIN_LIST; do
            pid=`pgrep -f "$APP_BIN "`
            if [ ! -z "$pid" ]; then
                kill -9 $pid
            fi
            echo "$APP_BIN killed"
        done
        echo "Done."
        ;;
    status)
        for APP_BIN in $APP_BIN_LIST; do
            pid=`pgrep -f "$APP_BIN "`
            if [ -z "$pid" ]; then
                echo "$APP_BIN is dead"
            else
                echo "$APP_BIN is alive"
            fi
        done
        echo "Done."
        ;;
    logrotate)
        echo "$APP_NAME: Rotating logs..."
        for APP_BIN in $APP_BIN_LIST; do
            pid=`pgrep -f "$APP_BIN "`
            if [ ! -z "$pid" ]; then
                kill -HUP $pid
            fi
            echo "$APP_BIN"
        done
        echo "Done."
        ;;
    *)
        echo "usage: $0 [start|stop|restart|kill|status|logrotate]"
        ;;
esac

