# -- coding: utf-8 --**

import urllib.request
import urllib.parse
import json
import time
import os
import sqlite3


url="http://192.168.2.254/stok=Changeme/ds"
headers={'Content-Type':'application/json; charset=UTF-8'}
data={"error_code":-1}
conn = sqlite3.connect('/srv/dev-disk-by-label-NAS/Share/test.db')
sqlite_cursor = conn.cursor()
LastWarning = False
IsWarning = False


def db_creat():
    global sqlite_cursor,conn
    sqlite_cursor.execute('''CREATE TABLE IF NOT EXISTS Router 
    (ID       INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    TIME     TEXT,
    MAC      TEXT,
    TYPE     TEXT,
    IP       TEXT,
    HOSTNAME TEXT
    );''')
    conn.commit()

def db_read():
    global sqlite_cursor,conn,LastWarning
    cursor = sqlite_cursor.execute('''SELECT MAX(ID),
       TIME,
       MAC,
       TYPE FROM Router;''')
    cow = cursor.fetchone()
    if(cow[3] != None):
        print("%s  eventid:%d,%s %s" %(cow[1],cow[0],cow[2],cow[3]))
        LastWarning = (cow[3].lower() == "up")
        print("LastStatus:",cow[3].lower()," LastWarning:",LastWarning)
    conn.commit()


def login():
    global url,sqlite_cursor,conn,data
    login_url="http://192.168.2.254/"
    param={"method":"do","login":{"password":"r9B8lg4U3wefbwK"}}

    #param = urllib.parse.urlencode(param)
    postdata = json.dumps(param).encode('utf-8') # data should be bytes
    try:
        # need Request to pass headers
        print(type(data))
        req = urllib.request.Request(login_url, postdata, headers)
        resp =urllib.request.urlopen(req)
        strstr=resp.read().decode("utf-8")
        print(strstr)
        data=json.loads(strstr)
        #print(resp.getheaders())
    except urllib.error.URLError as e:
    #print(len(strstr))
        print("login fail: ",e.reason)
        data["error_code"] = str(data["error_code"])+" "+ e.reason
        data={"error_code":-1}
    finally:
        if(data["error_code"]==0):
            url="http://192.168.2.254/stok="+data["stok"]+"/ds"
        else:
            sqlite_cursor.execute("INSERT INTO Router (IP,TYPE,MAC,TIME,HOSTNAME) VALUES ('0.0.0.0','Login Fail','00-00-00-00','"+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))+"','"+str(data["error_code"])+"');")
            conn.commit()
            conn.close()
            exit()

def get_info():
    global url,data
    param={"hyfi":{"table":["connected_ext"]},"hosts_info":{"table":"host_info","name":"cap_host_num"},"method":"get"}
    print("url:",url)
    postdata = json.dumps(param).encode('utf-8') # data should be bytes
    try:
        req = urllib.request.Request(url, postdata, headers)
        resp =urllib.request.urlopen(req)
        strstr=resp.read().decode("utf-8")
        #print(strstr)
        #print(resp.getheaders())
        data=json.loads(strstr)
        return True
    except (urllib.error.HTTPError,urllib.error.URLError) as e:
    #print(len(strstr))
        print("get_info fail: ",e.reason)
        data={"error_code":-1}
        return False

def print_info():
    global data,sqlite_cursor,conn,IsWarning
    IsWarning = False
    device_num = data["hosts_info"]["cap_host_num"]["host_num"]
    print("device_num:",device_num)
    i=0
    # {'host_info_5': {'mac': 'd8-2f-e6-59-01-a0', 'parent_mac': 'd4-83-04-2a-67-38', 'is_mesh': '0', 'wifi_mode': '0', 'type': '1', 'blocked': '0', 'ip': '192.168.2.100', 'hostname': 'Tmall%2DGenie', 'up_speed': '0', 'down_speed': '0', 'up_limit': '0', 'down_limit': '0', 'cfg_valid': '0', 'is_cur_host': '0', 'ssid': '', 'forbid_domain': '', 'forbid_time': '', 'plan_rule':[]}}
    print("MAC                 IP            类型   上传   下载 主机名")
    while(i<device_num):
        host_info_temp=data["hosts_info"]["host_info"][i]["host_info_"+str(i)] 
        if(host_info_temp["mac"]=="72-1f-51-74-aa-6a"):
            IsWarning = True
            print("Keep Warning!")
            if(not LastWarning):
                print("Start Warning!")
                sqlite_cursor.execute("INSERT INTO Router (IP,TYPE,MAC,TIME,HOSTNAME) VALUES ('"+host_info_temp["ip"]+"','up','"+host_info_temp["mac"]+"','"+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))+"','"+urllib.parse.unquote(host_info_temp["hostname"])+"');")
                conn.commit()
        info= ("%s %15s  %s   %6s %6s %s " %(host_info_temp["mac"]  , host_info_temp["ip"] ,host_info_temp["wifi_mode"],host_info_temp["up_speed"]  , host_info_temp["down_speed"],urllib.parse.unquote(host_info_temp["hostname"])))
        print(info)
        i=i+1
    if(not IsWarning and LastWarning):
        print("Start Safe!")
        sqlite_cursor.execute("INSERT INTO Router (IP,TYPE,MAC,TIME) VALUES ('0.0.0.0','down','72-1f-51-74-aa-6a','"+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))+"');")
        conn.commit()



db_creat()
while(True):
    if(data["error_code"]!=0):
        login()
    if(get_info()):
        db_read()
        print_info()
    time.sleep(55)
    if os.name == 'nt':  # 对于Windows系统
        os.system('cls')
    else:  # 对于Linux/Mac等系统
        os.system('clear')  # ANSI转义码




#mothod:POST
#url=http://192.168.2.254/
#param={"method":"do","login":{"password":"r9B8lg4U3wefbwK"}}
#response={"error_code":0, "stok":"Ad3w%2AnIx%7D0Hs%7B%7EoVI%2Eoe3%2B%5B%5B%2EO1gRZEo"}


#url=http://192.168.2.254/stok=sTP4M%2C7%3CA)%7BnO5%2CQTb%7D%7DH%7DZP60D0Tdb)/ds
#param={"hyfi":{"table":["connected_ext"]},"hosts_info":{"table":"host_info","name":"cap_host_num"},"method":"get"}
#response={"hyfi":{"connected_ext":[]},"hosts_info":{"host_info":[{"host_info_0":{"mac":"16-90-0b-23-52-97","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"0","blocked":"0","ip":"192.168.2.150","hostname":"J4125Openwrt","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"1","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_1":{"mac":"2e-6c-42-02-21-71","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"0","blocked":"0","ip":"192.168.2.150","hostname":"openmediavault","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_2":{"mac":"d8-b0-4c-b6-f7-37","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"0","blocked":"0","ip":"192.168.2.102","hostname":"HHT%5F10%5F2%5F6%5F246","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_3":{"mac":"00-90-4c-c5-12-38","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"1","type":"1","blocked":"0","ip":"192.168.2.175","hostname":"N1%5FWlan","up_speed":"1897","down_speed":"370","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_4":{"mac":"58-fb-84-1d-91-6f","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"1","type":"1","blocked":"0","ip":"192.168.2.103","hostname":"DESKTOP%2DJAA5TMO","up_speed":"1747","down_speed":"6188","up_limit":"0","down_limit":"0","cfg_valid":"0","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_5":{"mac":"d8-2f-e6-59-01-a0","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"1","blocked":"0","ip":"192.168.2.100","hostname":"Tmall%2DGenie","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"0","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_6":{"mac":"72-1f-51-74-aa-6a","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"1","blocked":"0","ip":"192.168.2.105","hostname":"HUAWEI%5FP30%5FPro%2De215f6cf8d","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"0","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_7":{"mac":"46-88-6f-e5-7a-6b","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"1","blocked":"0","ip":"192.168.2.101","hostname":"%E5%8C%BF%E5%90%8D%E4%B8%BB%E6%9C%BA","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_8":{"mac":"2c-3a-e8-00-97-81","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"1","blocked":"0","ip":"192.168.2.66","hostname":"dc1","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_9":{"mac":"dc-4f-22-48-c2-2a","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"1","blocked":"0","ip":"192.168.2.82","hostname":"esphome","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"0","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}},{"host_info_10":{"mac":"00-f1-f3-20-99-55","parent_mac":"d4-83-04-2a-67-38","is_mesh":"0","wifi_mode":"0","type":"0","blocked":"0","ip":"192.168.2.125","hostname":"PVE","up_speed":"0","down_speed":"0","up_limit":"0","down_limit":"0","cfg_valid":"1","is_cur_host":"0","ssid":"","forbid_domain":"","forbid_time":"","plan_rule":[]}}],"cap_host_num":{"host_num":11,"guest_num":0}},"error_code":0}
