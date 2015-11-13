rm -f netcat-channel
mkfifo netcat-channel
trap "rm -f netcat-channel" EXIT
while true
do
	cat netcat-channel | nc -l 3000 > >( # Here starts the netcat parser.
	export REQUEST=
	while read line
	do
		line=$(echo "$line" | tr -d '[\r\n]')

		if echo "$line" | grep -qE '^POST /' # Only "POST /" are served.
		then
			REQUEST=$(echo "$line" | cut -d ' ' -f2) # The request data.
		elif [ "x$line" = x ] # Empty line or EOF.
		then
			if echo $REQUEST | grep -qE '^/'
			then
				echo $REQUEST | ./whitelist-verify > netcat-channel
			else
				echo "Broken request"
			fi
		fi
	done
	)
done
