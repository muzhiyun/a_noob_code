#coding:gb2312
import cookielib
import urllib2
import urllib
import re
#import sys
#import smtplib                               
#from email.mime.text import MIMEText
#from email.header import Header
#import gzip
#import StringIO
#import chardet
 
url = 'http://jwxt.xsyu.edu.cn/eams/stdSyllabus!search.action'
postdata = urllib.urlencode({
            'lesson.project.id':'1',
			'lesson.semester.id':'45',
			'pageNo':'2',
			'pageSize':'1000',
           })
user_agent = 'Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)'      
headers = { 'User-Agent' : user_agent }
request = urllib2.Request(url,postdata)
response=urllib2.urlopen(request)
r = response.read()
print r
#data = StringIO.StringIO(r)

#gzipper = gzip.GzipFile(fileobj=data)
#print gzipper
#html = gzipper.read()
#print html
# 返回值
#print test


#显示用
#paswrd=re.findall('<tr>([\s\S]*?)</tr>',test)
#paswrd.encode=('utf-8')
#print paswrd
# 匹配所有超链接
#paswrd=re.findall('<a.*?href="[^"]*".*?>([\S\s]*?)</a>',back)
# 匹配所有超链接标题
#username=re.findall('<a.*?href="http://www.******.com/bbs/space-uid-342911.html".*?>([\S\s]*?)</a>',back)