#!/bin/bash

src_exe=`grep ^"module_name=" ../conf/serv_info.conf | awk -F '=' '{print $2}'`
exe=`grep ^"project=" ../conf/module.conf | awk -F '=' '{print $2}'`_${src_exe}
serv_port=`grep ^"${src_exe}_serv_port=" ../conf/module.conf | awk -F '=' '{print $2}'`
reg_port=`grep ^"${src_exe}_reg_port=" ../conf/module.conf | awk -F '=' '{print $2}'`

num=`ps -ef |awk '{print $8}'| grep "^./$exe$" | wc -l`

if [[  $num -lt 1 ]]
then
	echo "$exe not exist!"
else 
	echo "$exe is OK!"
fi


exist=`netstat -nal | grep LISTEN | grep $serv_port | grep -v grep | wc -l`
if [[ $exist == 1 ]]
then 
	echo "ServPort: $serv_port listen OK!"
else
	echo "ServPort: $serv_port is not listening!"
fi

if [ -z "${reg_port}" ]
then
    echo "no reg_port."
    exit
fi

exist=`netstat -nal | grep LISTEN | grep $reg_port | grep -v grep | wc -l`
if [[ $exist == 1 ]]; then
    echo "RegPort $reg_port listen OK!"
else
    echo "Regport $reg_port is not listening!"
fi

