#第二封python代码
#为了爬垃圾图书馆的借阅系统写的半成品   垃圾代码  又臭又长
#死于中文乱码 不接受吐槽 over
#2017-03-25    整理时间 2018-06-19


#coding:utf-8
#encoding:utf-8
import urllib
import urllib2
import re
from bs4 import BeautifulSoup

num=500005
mu=1
ma=1
while num<500006:
    num= "%07d" % num
    num=str(num)
    url = "http://202.200.87.00/wx_OilUniversity_rtm/index.php/index/Search/bookContent/recCtrlId/01h"+num  #我会告诉你真实IP吗 笑话
    request=urllib2.Request(url)
    res=urllib2.urlopen(request)
    test=res.read()
    res_ta=r'<a.*?>(.*?)</a>'
    m_a =re.findall(res_ta,test,re.S|re.M)
    for aa in m_a:
       if ma==1:
          ma1=aa
       if ma==2:
          ma2=aa
          print ma2
       if ma==3:
          ma3=aa
       if ma==4:
          ma4=aa
       if ma==5:
          ma5=aa
       if ma==7:
          ma7=aa
       ma+=1
    res_td=r'<td>(.*?)</td>'
    m_td =re.findall(res_td,test,re.S|re.M)
    for nn in m_td:
       if mu==1:
          mu1=nn
       if mu==2:
          mu2=nn
       if mu==3:
          mu3=nn
       if mu==4:
          mu4=nn
       if mu==6:
          mu6=nn
       if mu==7:
          mu7=nn
       if mu==8:
          mu8=nn
       if mu==9:
          mu9=nn
       if mu==11:
          mu11=nn
       if mu==12:
          mu12=nn
       if mu==13:
          mu13=nn
       if mu==14:
          mu14=nn
       mu+=1
    num=int(num)
    num+=1
print("stop")
