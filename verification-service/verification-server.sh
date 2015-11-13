rm -f netcat-channel
mkfifo netcat-channel
trap "rm -f netcat-channel" EXIT
while true
do
	cat netcat-channel | nc -l 3000 > >( # Here starts the netcat parser.
	export REQUEST_PATH=
	while read line
	do
		line=$(echo "$line" | tr -d '[\r\n]')
		if echo "$line" | grep -qE '^POST /' # Only "POST /" are served.
		then
			REQUEST_PATH=$(echo "$line" | cut -d ' ' -f2) # Extract the request path.
		fi

		if echo "$line" | grep -qE 'id' # Contains id keyword in the line, which is part of the JSON.
		then
			if echo $REQUEST_PATH | grep -qE '^/'
			then
				echo "$line" | ./whitelist-verify > netcat-channel
			else
				echo "Broken request"
				echo $REQUEST_PATH
			fi
		fi
	done
	)
done
