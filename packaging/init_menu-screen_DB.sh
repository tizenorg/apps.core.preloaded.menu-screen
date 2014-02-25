#!/bin/sh
source /etc/tizen-platform.conf
if [ ! -d $1/dbspace ]
then
       mkdir -p $1/dbspace
fi
if [ ! -d $1/shortcut ]
then
       mkdir -p $1/shortcut
else
       rm -rf $1/shortcut/*
fi
sqlite3 $1/dbspace/.menu_screen.db 'PRAGMA journal_mode = PERSIST;
	create table if not exists shortcut (
		ROWID INTEGER PRIMARY KEY AUTOINCREMENT,
		appid TEXT,
		name TEXT,
		type INTEGER,
		content_info TEXT,
		icon TEXT
	);
'
users_gid=$(getent group $TZ_SYS_USER_GROUP | cut -f3 -d':')

INHOUSE_ID="$users_gid"
chown -R $INHOUSE_ID:$INHOUSE_ID $1
chown root:$INHOUSE_ID $1/dbspace/.menu_screen.db
chown root:$INHOUSE_ID $1/dbspace/.menu_screen.db-journal

chmod 660 $1/dbspace/.menu_screen.db
chmod 660 $1/dbspace/.menu_screen.db-journal

