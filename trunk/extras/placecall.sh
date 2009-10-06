#!/bin/bash
# Oct 06 2009 <penschuck>

# some vars to fill
HOST=""			 	# host where asterisk manager is running on
PORT=5038			# default is 5038
USER="caller"
PASS="secret"
EXT="";				# the extension of the phone to use
CONTEXT="from-internal"

# here we go
if [ "$1" = "" ]; then
	echo "Usage: placecall.sh <phonenumber>"
	exit
fi

NUMBER=$1
CID="Call <$NUMBER>"
exec 3<>/dev/tcp/$HOST/$PORT

echo -e "Action: login\r\n\
Events: off\r\n\
Username: $USER\r\n\
Secret: $PASS\r\n\r\n\
Action: originate\r\n\
Channel: SIP/$EXT\r\n\
WaitTime: 30\r\n\
CallerId: $CID\r\n\
Exten: $NUMBER\r\n\
Context: $CONTEXT\r\n\
Priority: 1\r\n\r\n\
Action: Logoff\r\n\r\n" >&3
