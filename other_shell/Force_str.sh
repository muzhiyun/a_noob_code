lock=$(cat /sys/power/wake_lock)

#echo $lock

for lockitem in $lock
do
	#echo "print"
	#echo $locktime
	echo $locktime > /sys/power/wake_unlock
done
