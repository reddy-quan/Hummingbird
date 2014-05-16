#!/bin/sh

source send_cmd_pipe.sh

#if ifconfig -a | grep eth0; then
#	ifconfig eth0 up
#	sleep 1
#else
#	SEND_CMD_PIPE_FALL $3
#	exit 1
#fi

#var_ping=$(ping -c 4 www.baidu.com);
#num_ping=$(echo $var_ping | grep -c "bytes from");
	
#if [ $num_ping -eq 1 ]; then
#	sleep 1
#	SEND_CMD_PIPE_OK $3
#	ifconfig eth0 down
#	exit 1
#else
#	SEND_CMD_PIPE_FAIL $3
#	exit 1		
#fi

#ifconfig eth0 down
#SEND_CMD_PIPE_FAIL $3

if ifconfig -a | grep eth0; then
	ifconfig eth0 up
	if [ $? -ne 0 ];then

		SEND_CMD_PIPE_FALL $3
		exit 1
    fi	

	var_ping=$(ping -c 4 127.0.0.1);

	#echo $var_ping | grep -c "0% packet loss"
	num_ping=$(echo $var_ping | grep -c "0% packet loss");
	
	if [ $num_ping -eq 1 ]; then
		echo "run............."
		SEND_CMD_PIPE_OK $3
	else
	
	
			SEND_CMD_PIPE_FAIL $3

		exit 1
		
	fi

	ifconfig eth0 down
	if [ $? -ne 0 ]; then

		SEND_CMD_PIPE_FALL $3
		exit 1
	fi
	
else

	SEND_CMD_PIPE_FAIL $3
	exit 1
fi


	
	
	
	
	
	
	
	