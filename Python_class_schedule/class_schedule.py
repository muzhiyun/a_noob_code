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
# ����ֵ
#print test


#��ʾ��
#paswrd=re.findall('<tr>([\s\S]*?)</tr>',test)
#paswrd.encode=('utf-8')
#print paswrd
# ƥ�����г�����
#paswrd=re.findall('<a.*?href="[^"]*".*?>([\S\s]*?)</a>',back)
# ƥ�����г����ӱ���
#username=re.findall('<a.*?href="http://www.******.com/bbs/space-uid-342911.html".*?>([\S\s]*?)</a>',back)