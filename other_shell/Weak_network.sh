sum=0
while :
do
rand_time=`expr $RANDOM / 80`; 
echo "  sleep ${rand_time}s"; 
sleep ${rand_time};

rand_case=`expr $RANDOM % 3 + 1`
echo "  network swtich ${rand_case}"; 

#rand_case=3

case $rand_case in
	1)
	curl 'http://192.168.124.1/goform/aspForm'   \
	-H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9'   \
	-H 'Accept-Language: zh-CN,zh;q=0.9'   \
	-H 'Cache-Control: no-cache'   \
	-H 'Connection: keep-alive'   \
	-H 'Content-Type: application/x-www-form-urlencoded'\
	-H 'Cookie: PSWMOBILEFLAG=true; USERLOGINIDFLAG=; LOGIN_PSD_REM_FLAG='   \
	-H 'Origin: http://192.168.124.1'   \
	-H 'Pragma: no-cache'   \
	-H 'Referer: http://192.168.124.1/mobile_access_ipqos_submit.asp'  \
	-H 'Upgrade-Insecure-Requests: 1'   \
	-H 'User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36'   \
	--data-raw 'CMD=ipqos_lanip_editlist&GO=mobile_access_ipqos_submit.asp&param=192.168.124.2%3B192.168.124.2%3B0%3B560%3B2%3B%3B1%3B0%3B2&inter_ip_flux_en=1&statistics_cycle=5&ipqos_lanip_en=1&ipqos_lanip_bounded_en=1'   \
	--compressed   \
	--insecure
	;;
	2)
	curl 'http://192.168.124.1/goform/aspForm' \
  -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9' \
  -H 'Accept-Language: zh-CN,zh;q=0.9' \
  -H 'Cache-Control: no-cache' \
  -H 'Connection: keep-alive' \
  -H 'Content-Type: application/x-www-form-urlencoded' \
  -H 'Cookie: PSWMOBILEFLAG=true; USERLOGINIDFLAG=; LOGIN_PSD_REM_FLAG=' \
  -H 'Origin: http://192.168.124.1' \
  -H 'Pragma: no-cache' \
  -H 'Referer: http://192.168.124.1/mobile_access_ipqos_submit.asp' \
  -H 'Upgrade-Insecure-Requests: 1' \
  -H 'User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36' \
  --data-raw 'CMD=ipqos_lanip_editlist&GO=mobile_access_ipqos_submit.asp&param=192.168.124.2%3B192.168.124.2%3B0%3B960%3B2%3B%3B1%3B0%3B2&inter_ip_flux_en=1&statistics_cycle=5&ipqos_lanip_en=1&ipqos_lanip_bounded_en=1' \
  --compressed \
  --insecure
	;;
	3)
	curl 'http://192.168.124.1/goform/aspForm'   \
	-H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9'   \
	-H 'Accept-Language: zh-CN,zh;q=0.9'   \
	-H 'Cache-Control: no-cache'   \
	-H 'Connection: keep-alive'   \
	-H 'Content-Type: application/x-www-form-urlencoded'   \
	-H 'Cookie: PSWMOBILEFLAG=true; USERLOGINIDFLAG=; LOGIN_PSD_REM_FLAG='   \
	-H 'Origin: http://192.168.124.1'   \
	-H 'Pragma: no-cache'     \
	-H 'Referer: http://192.168.124.1/mobile_access_ipqos_submit.asp'  \
	-H 'Upgrade-Insecure-Requests: 1'   \
	-H 'User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.88 Safari/537.36'   \
	--data-raw 'CMD=ipqos_lanip_dellist&GO=mobile_access_ipqos_submit.asp&param=2%3B&inter_ip_flux_en=1&statistics_cycle=5&ipqos_lanip_en=1&ipqos_lanip_bounded_en=1'   --compressed   \
	--insecure
	;;
esac

let "sum+=1"
echo "sum=$sum"
done