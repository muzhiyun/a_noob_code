token="79099,eff3589d73e2f69d2f0a957f19196576"
domain="64519959"
record="400904259"
#hk2p 400903563
#t1 400904259
pv6=$(ip addr show wlan0 | grep inet6 | sed 's/\/.*//g' | awk '{print $2}'  | grep 240e |head -1)
oldip=$(curl -s -X POST "https://dnsapi.cn/Record.Info" -d "login_token=$token&format=json&domain_id=$domain&record_id=$record" |jq '.record.value'|sed 's/\"//g')
echo $pv6
echo $oldip
if [ "$pv6" != "$oldip" ] ;then
{
name=$(curl -s -X POST "https://dnsapi.cn/Record.Info" -d "login_token=$token&format=json&domain_id=$domain&record_id=$record" |jq '.record.sub_domain'|sed 's/\"//g')
curl -s  -X POST "https://dnsapi.cn/Record.Modify" -d "login_token=$token&format=json&domain_id=$domain&record_id=$record&record_type=AAAA&record_line=%e9%bb%98%e8%ae%a4&value=${pv6}&sub_domain=$name"
}
else
	echo "Same value,not updated"
fi