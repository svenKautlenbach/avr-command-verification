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

      if echo "$line" | grep -qE '^POST /' # if line starts with "GET /"
      then
        REQUEST=$(echo "$line" | cut -d ' ' -f2) # extract the request
      elif [ "x$line" = x ] # empty line / end of request
      then
        if echo $REQUEST | grep -qE '^/'
	then
		echo $REQUEST | ./avr-whitelist-verify > netcat-channel
	else
		echo "jombe"
        fi
      fi
    done
  )
done
