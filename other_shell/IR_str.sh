sum=0
while :
do
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 15s"
sleep 15
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 20s"
sleep 20
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 25s"
sleep 25
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 30s"
sleep 30
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 35s"
sleep 35
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 40s"
sleep 40
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 45s"
sleep 45
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 50s"
sleep 50
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 55s"
sleep 55
curl -X POST http://192.168.0.117/switch/my_switch_off/toggle
echo "sleep 60s"
sleep 60
let "sum+=1"
echo "sum=$sum"
done