SCHEME=$1
DATE=`date +%s`

nc localhost 4444 -q0 << EOF
reset run
EOF
stty -F /dev/ttyUSB0 115200
cat /dev/ttyUSB0 > ${DATE}-tpm-${SCHEME}.log & 
CATPID=$!
echo CATPID is ${CATPID}
sleep 5
ssh mbedpi killall -9 command_test
ssh mbedpi tpmfirmware/TPMCmd/test/command_test -S "${SCHEME}" | tee ${DATE}-pi-${SCHEME}.log
kill -9 ${CATPID}
