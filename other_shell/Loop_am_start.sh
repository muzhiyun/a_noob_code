num=1
while true; do
  am start -n com.microsoft.office.powerpoint/com.microsoft.office.apphost.LaunchActivity
  sleep 20
  echo $num
  num=`expr $num + 1`
  input keyevent "KEYCODE_HOME"
  sleep 20
done  
