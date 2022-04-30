<?php

header('Access-Control-Allow-Origin:http://127.0.0.1:8080'); 
header('Access-Control-Allow-Methods:POST'); 
header('Access-Control-Allow-Headers:X-Upload-Token');
header('Access-Control-Allow-Credentials: true');

if(isset($_SERVER['REQUEST_METHOD']) && strtoupper($_SERVER['REQUEST_METHOD'])=='OPTIONS')
{
exit();
}


if(isset($_FILES["file"]))
{
	$allfile= $_FILES["file"];
}
else
{
    $ret = array(
        "msg"=> "Please select less a file",
        "code"=> 403,
        "data"=>array(
            "errFile"=>$_FILES["file"],
            "succMap"=>"")
    );
    echo json_encode($ret);
	exit;
}

#print_r($allfile);
$succMap=array();

for ($i =0;$i <count($allfile[name]); $i++ ){
    if($file['error'][$i] == 0){

        $file_name = $allfile["name"][$i];
        $tmp_file = $allfile["tmp_name"][$i];

        move_uploaded_file($tmp_file, 'image/'.$file_name);
        $succMap[$file_name]="http://192.168.2.175/image/".$file_name;
        $retmsg="";
        $retcode=0;
        $errFiles=array();
    }
    else
    {
        $retmsg="error";
        $retcode=403;
        $errFiles=array();
    }
}
    $ret=array("msg"=>$retmsg,"code"=>$retcode,data=>(array("errFile"=>$errFiles,"succMap"=>$succMap)));
    echo json_encode($ret,JSON_UNESCAPED_SLASHES);





    //http://192.168.2.175/upload.php
    //https://ld246.com/upload/client

    // {
    //     "msg": "",
    //     "code": 0,
    //     "data": {
    //         "errFiles": [],
    //         "succMap": {
    //             "Cache_729606b75b511a22..png": "https://b3logfile.com/file/2022/04/Cache_729606b75b511a22.-baed73d9.png",
    //             "Cache_729606b75b511a22..jpg": "https://b3logfile.com/file/2022/04/Cache_729606b75b511a22.-b5dab494.jpg"
    //         }
    //     }
    // }







?>