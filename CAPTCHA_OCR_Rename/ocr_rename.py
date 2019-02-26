# -*- coding:utf-8 -*- 
from aip import AipOcr
import os, re
import time
import sys
import json
import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

""" 你的 APPID AK SK """
APP_ID = '*********'
API_KEY = '**************'
SECRET_KEY = '***********'

client = AipOcr(APP_ID, API_KEY, SECRET_KEY)


""" 读取图片 """
def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()

"""ocrname去空格"""
def remove_blank(ocrname):
	aftername = ''
	"""取文件名中每一个字符做验证"""
	for i in ocrname:
		"""是否英文字符"""
		if 'A'<=i and i<='z':
			"""转为小写字母"""
			aftername =str.lower(aftername)
			aftername =aftername+i
	return aftername

"""循环遍历文件名"""
for item in os.listdir('.'):
	if (re.match(".*.py", item)):
		continue;
	print item
     	image = get_file_content(item)
	"""休眠 Api的QPS相关"""
	time.sleep(1)
	""" 调用通用文字识别（高精度版） """
	num = client.basicAccurate(image);
	print num
	"""提取返回结果"""
	ocrname = str(num['words_result'][0]['words'])
	print str(num['words_result'])
	if str(num['words_result']) == "" :
	    continue;
	aftermbk = remove_blank(ocrname)
	print ocrname,aftermbk
	#len(aftermbk)
	if 4>len(aftermbk) or len(aftermbk)>5:
		"""调用另一个接口 通用文字识别(普通版)"""
		base = client.basicGeneral(image);
		basename = str(base['words_result'][0]['words'])
		aftermbk = remove_blank(basename)
		print base,basename
	#print ocrname,aftermbk
	if 4>len(aftermbk) or len(aftermbk)>5:
		continue;
	print ocrname,aftermbk
	newname = aftermbk +"_"+str(time.time())+".jpg"
	#print item,newname,type(newname)
	os.rename(item,newname)
	os.system("mv "+newname+" ../after/")
	print "The image \"" + item + "\" rename as -->" + newname

	#if ocrname == basename:
		#newname = ocrname +"_"+str(time.time())+".jpg"
		#print newname
		#os.system("cp "+item+" /home/ubunt"+newname)
		#print "The image \"" + item + "\" is " + ocrname
		#print "And rename as -->" + newname
	#else:
		#newname = ocrname +"______"+basename+".jpg"
		#os.system("cp "+item+" "+newname)
		#os.system("mv "+newname+" /home/ubuntu/")
		#print "And rename as -->" + newname
		#os.rename(item, newname)
