#new=$(curl -s1 https://android.myapp.com/myapp/detail.htm?apkName=com.tencent.tmgp.sgame | grep -o '?fsname=.*&'|head -1)
url=$(curl -s1 https://android.myapp.com/myapp/detail.htm?apkName=com.tencent.tmgp.sgame | grep -o 'downUrl:".*\"' | awk -F "[\"\"]" '{print $2}')
new=$(curl -s1 https://android.myapp.com/myapp/detail.htm?apkName=com.tencent.tmgp.sgame | grep -o 'com\..*\.apk'|head -1)
old=$(ls /srv/dev-disk-by-label-data/Share/Aria下载/|grep com.tencent.tmgp.sgame*)
echo "************************ Debug info ****************************"
echo "The URL is \""$url"\""
echo "The new filename is \""$new"\""
echo "The old filename is \""$old"\""
if [ "$new" != "$old" ];then
{
echo -e "\n\nDifferent, start to download \n\n"
echo -e "****************** start to download **********************\n"
cd /srv/dev-disk-by-label-data/Share/Aria下载/
rm -rf $old
wget  -c --user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36" -O $new $url
echo "********************** finish ******************************"
}
else
{
echo -e "\n\nSame, do not download"
echo "********************** finish ******************************"
}
fi


