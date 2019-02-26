Project="https://github.com/opengapps/arm64"
#Project="https://github.com/muzhiyun/PHP_MySQL"
word="5.1"
word2="nano"
url=$(curl -s1 ${Project}/releases/latest|grep -o '<a href=\".*\"'| awk -F "[\"\"]" '{print $2}')
#echo "The latest url is "$url
check=$(echo ${url#*releases})
if [ "$check" != "" ];then
	tag=$(echo ${check#*tag/})
	echo "The new version tag is "$tag
else
	echo "……………………………………………………………………………………………………………………………………………"
	echo "Wrong Wrong Worng!!!!!"
	echo ""
	echo "This Project is no releases.Please check The Project url"
	echo "……………………………………………………………………………………………………………………………………………"
	exit
fi

namelist=$(curl -s1 $url|grep -o 'text-bold\">.*\.zip</span>'|awk -F "[><]" '{print $2}')

#echo "…………………………………………………………………………………………………………………………………………"
#echo "The all name is"
#for allname in $namelist;
#do { 
#echo $allname
#}
#done

echo "…………………………………………………………………………………………………………………………………………"
echo "The choosed is"
for name in $namelist;
do {
choose=$(echo $name|grep -o ".*${word}.*${word2}.*")
#可能存在多个平台 进行关键字筛选
if [ "$choose" != "" ];then
Part="$choose"
echo $Part
fi
}
done

downurl="$Project/releases/download/$tag/$Part"
#拼接下载地址
echo "………………………………………………………………………………………………………………………………………………"
echo "The final downurl is "
echo "$downurl"
#wget  -c --user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36" -O $Part $downurl
echo "………………………………………………………………finish…………………………………………………………………"
