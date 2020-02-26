#!/bin/bash

function activation()
{
  if [ $status == "active" ]
then
   echo "Active"
   sudo service postgresql status
else 
    [ $status == "inactive" ]
   echo "inactive"
   sudo service postgresql start
   sudo service postgresql status
fi
}

function query_select()
{
#query=$(echo 'postgres' | su -c "psql -U postgres -c \"SELECT 1\"" -m postgres)

query=$(sudo su - postgres -c "psql -U postgres -c \"SELECT 1\"")

if [ -z "$query" ]
then
	echo "Inactive"
        sudo service postgresql start
   	#sudo service postgresql status
else
        echo "Active"
   	#sudo service postgresql status
fi

}

function backup()
{
  su -c "pg_dump $1 | gzip > ~postgres/$1.gzip" -m postgres
}

function restore()
{
  su -c "gunzip -c /var/lib/postgresql/$1.gzip | psql $1" -m postgres
}

# $1 - db name
echo DB name for backup: $1

#input mode : activation, backup, restore
echo Script mode: $2
echo Script User: $3

Substring=$(/etc/init.d/postgresql status | grep Active )
status=$(echo $Substring | cut -d' ' -f'2')

if [ $2 == "activation" ]
then
   #activation $status
   query_select $3
elif [ $2 == "backup" ]
then
   echo "BackUp"
   backup $1
elif [ $2 == "restore" ]
then
  echo "Restore" 
  restore $1
fi

