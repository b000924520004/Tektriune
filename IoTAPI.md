# CoreServiceAzure

- [CoreAPI](#coreapi)
  - [IoTAPI (JSON-RPC 2.0)](#iotapi (json-rpc-20))
    - [1、如何進行註冊裝置及啟用服務(一階資訊)](#1如何進行註冊裝置及啟用服務(一階資訊))
      - [1.1、註冊裝置及獲取相關資訊(公開API)](#11註冊裝置及獲取相關資訊(公開api))
      - [1.2、啟用服務](#12啟用服務)   
    - [2、如何取得啟用服務後的後續資訊及使用相關服務](#2如何取得啟用服務後的後續資訊及使用相關服務)
      - [2.1、如何加入群組並獲得Blob及MQTT之連接資訊(二階資訊)進行連接](#21如何加入群組並獲得blob及mqtt之連接資訊(二階資訊)進行連接)
      - [2.2、如何透過mqtt協定傳送訊息(掃Tag過濾各裝置)](#22如何透過mqtt協定傳送訊息(掃tag過濾各裝置))
      - [2.3、如何透過mqtt協定更新裝置Tag屬性](#23如何透過mqtt協定更新裝置tag屬性)
      - [2.4、如何透過mqtt協定將訊息發送給特定裝置及接收訊息](#24如何透過mqtt協定將訊息發送給特定裝置及接收訊息)
      - [2.5、如何透過mqtt協定訂閱上下線通知](#25如何透過mqtt協定訂閱上下線通知)
      - [2.6、如何透過mqtt協定發送一則公告訊息](#26如何透過mqtt協定發送一則公告訊息)
      - [2.7、如何透過mqtt協定發送保留訊息以及取得保留訊息](#27如何透過mqtt協定發送保留訊息以及取得保留訊息)
      - [2.8、如何透過Azure Storage Client Library上傳或下載檔案(Blob)](#28如何透過azure-storage-client-library上傳或下載檔案blob)
      - [2.9、如何透過mqtt協定定期發送裝置心跳](#29、如何透過mqtt協定定期發送裝置心跳)
    - [3、如何結束服務](#3、如何結束服務)
      - [3.1、結束服務](#31、結束服務)

---

# CoreAPI   

## IoTAPI (JSON-RPC 2.0)   

[URL] [HOST domain]/IoT  
**測試**用HOST domain為https://coreapi20170124121347.azurewebsites.net   
**正式**用HOST domain為https://api.habookaclass.biz

---

JSON-RPC之參數，若沒特別註明，則都為**字串**格式。

---
除非特別註明是公開API，否則都需要使用**Token**來認證。  
另外若是Token過期，http_code還是會收到200，但是所收到的內容會是  

   ```cs  
   {
        "id" : 1,
        "jsonrpc" : "2.0",
        "result"  :"null",
        "error" :
        {
            "code" : -32600,
            "message" : "Unauthorized",
            "data" : null
        }
    }
   ```     
---

   MQTT傳訊部分以下列代號表示  

```cs
--> data sent to Server
<-- data sent to Client
```


### 1、如何進行註冊裝置及啟用服務(一階資訊)   

首先，若要使用此服務，**必須**先跟CoreService管理員申請以下資料。   
1.應用程式識別碼   
2.驗證碼   
3.驗證碼之版本號   

#### 1.1、註冊裝置及獲取相關資訊(公開API)  

　　此API會將準備要使用服務之裝置註冊至Azure IoT內，並回傳傳送**非即時**訊息之資訊及進入CoreService須驗證之API的Token。   
 
[Method] Regist  
[Param]

```cs   
"deviceId"             : "[裝置Id][1]"(唯一),
"clientId"             : "應用程式識別碼"(由CoreService管理員配發),
"verificationCode"     : "驗證碼"(由CoreService管理員配發),
"verificationCodeVer"  : "驗證碼版本號"(由CoreService管理員配發),
"productCode"          : "產品代號",
"extraInfo"(JObject)[2]: 
    {
        "os"          : "裝置OS"(非必要),
        "model"       : "裝置機型"(非必要)
    }
```
  [1]   
  區分大小寫的字串，最長為 128 個字元，可使用 UTF8 7 位元英數字元和 {'-' , ':' , '.' , '+' , '%' , '_' , '#' , '*' , '?' , '!' , '(' , ')' , ' ,' , '=' , '@' , ';' , '$' , '''}   
  [2]   
  os的部分請填入作業系統，如windows、ios、andriod、linux。   
  model的部分請填入裝置型別，如pc、web、mobile、server。


[Return]

   ```cs  
{
	"token" : "Bearer ..."(進入須要驗證之API所須的Token),
	"http"  : 
	{
		"authorization":"SharedAccessSignature sr=[sasToken]"(https認證用),
		"send":"https://[IotHostName]/devices/{deviceId}/messages/events?api-version=[2016-11-14]"(https URL)
	}      
}
http_code:200  
  
  ```  
    
---   
   
#### 1.2、啟用服務   
**由服務發起者呼叫**  
   
呼叫此函數即會創建IoT群組(6位數數字亂碼)。

請服務發起者將此六碼以及跳轉呼叫**JoinGroup**之函式方法寫入QRCode內。  


[Method] CreateGroup  
[Param]

```cs
"deviceId" : "裝置Id"
"doBoundGroupNum(bool)" : true or false (true為綁定一隨機groupNum，false為解綁) 
```   
[Return]

```cs  
{
    {群組號碼} 
}
http_code:200
   ```     
      
　　綁定之依據為**deviceId**，若第一次進行綁定群組號碼，則會隨機生成一組群組號碼並且進行綁定動作，
爾後該deviceId進行啟用服務時，所取得的群組號碼即會是第一次進行綁定所產生的號碼。


---

### 2、如何取得啟用服務後的後續資訊及使用相關服務 
#### 2.1、如何加入群組並獲得Blob及MQTT之連接資訊(二階資訊)進行連接   

**由欲加入群組之裝置呼叫**   
   

[Method] JoinGroup  
[Param]  

  ```cs
"deviceId" : "裝置Id"
"groupNum" : "群組號碼"
  ```   
[Return]  

```cs  
{
    "blobContainerSasUri" : "https://..."(blob的連線字串，效期1Day)
    "mqtt"(mqtt的連接資訊)
    {
        "connectInfo":
        {
            "brokerHostName" : "[IotHostName].azure-devices.net",
            "username"       : "[IotHostName].azure-devices.net/{deviceId}/api-version=[api-version]",
            "password"       : "SharedAccessSignature sr=[sasToken]",
            "clientID"       : "{deviceId}"
        },
        "publishTopic":
        {
            "getTwin"   : "$iothub/twin/GET/?$rid=0",(取得裝置所屬Tag)
            "patchTwin" : "$iothub/twin/PATCH/properties/reported/?$rid=0",(更新裝置所屬Tag)
            "sendMsg"   : "devices/{deviceId}/messages/events/DeviceId={deviceId}&groupNum={groupNum}&Endpoint={endpoint}",(傳送訊息的接點)
            "sendDirectMethod" : "$iothub/methods/res/200/?$rid="(回覆直接方法所使用之接點)
        },
        
        "subscribeTopic":
        {
            "receiveMsg"  : "devices/{deviceId}/messages/devicebound/#",(接收C2D的訊息)
            "receiveTwin" : "$iothub/twin/res/#",(接收由getTwin回傳的裝置Tag)
            "receiveDirectMethod" : "$iothub/methods/POST/directMethod/#"(接收透過直接方法傳進的訊息)
        }
     }
}
http_code:200 
```     

進行mqtt的連線時，**cleanSession**欄位請輸入**false**，這樣裝置一上線才能收到在Queue裡的訊息。

---  

#### 2.2、如何透過mqtt協定傳送訊息(掃Tag過濾各裝置)
　　**注意**，透過掃Tag在訊息計量部分會較一般透過deviceId傳送來的多，且訊息延遲時間較久，但透過掃Tag來傳送訊息是較靈活較方便，建議使用在比較不即時性的應用上。

　　首先先透過2.1小節所獲得之mqtt資訊內的connectInfo來連線至Azure IoT。
其中mqtt連線資訊內publishTopic的sendMsg，為Publish訊息的Topic端點，此**預設**為傳送給有加入**六碼群組**的**所有**成員。   
Ex:

```cs
devices/{deviceId}/messages/events/DeviceId={deviceId}&groupNum={groupNum}&Endpoint={endpoint}
```

　　若要傳送給單一或多人等，則必須於mqtt連線資訊內publishTopic的sendMsg端點"devices/{deviceId}/messages/events/DeviceId=\{deviceId}&groupNum=\{groupNum}&Endpoint=\{endpoint}"後加上**&$.to=\{JSON Format}**。   

　　如何更新裝置本身所屬之Tag，請參考2.3小節。

Ex:   
要將訊息傳給某個key等於group且value等於G1的裝置。

```cs
-->
JObject jObject = new JObject()
{
    new JProperty("group","G1")
};

devices/{deviceId}/messages/events/DeviceId={deviceId}&groupNum={groupNum}&Endpoint={endpoint}&$.to={jObject.ToString()}
```
  
* 其中value若有多組項目，則請以**逗點**分隔開。 Ex: A,B,C   
* 目前.To屬性內有幾個是CoreService之保留字，localIp、connectionState、broadcast、deviceid、keepmessage、getkeepmessage、deletegroup。 
* 若.To屬性所夾帶的JSON內有**保留字**以及其他**非保留字**(自訂的tagkey等)，系統只會處理**保留字，非保留字**則**不處理**。
* 若.To屬性所夾帶的JSON內有多個**保留字**，則優先處理第一個掃到的保留字，第二個之後的保留字則忽略**不處理**，故推送一則資訊則處理一個保留字的工作，若要處理多個保留字，則是分批推送。
* 由於會掃Tag過濾各裝置，故處理時間會較長，且掃一次Tag會消耗一次的訊息計數。  

---  


#### 2.3、如何透過mqtt協定更新裝置Tag屬性

　　此裝置Tag屬性用來更新一些較常變動之資訊。    

　　首先，裝置會訂閱 $iothub/twin/res/#，以接收作業的回應。 然後，它會傳送**空白訊息**給主題 $iothub/twin/GET/?$rid=\{request id}，其中已填入要求識別碼的值。 服務接著會使用和要求相同的要求識別碼，傳送內含關於 $iothub/twin/res/\{status}/?$rid=\{request id} 主題之裝置對應項資料的回應訊息。

　　例如，透過mqtt推送一則空訊息至$iothub/twin/GET/?$rid=\{request id}，則會收到該裝置目前的Tag資訊。   

　　若要更新Tag則對$iothub/twin/PATCH/properties/reported/?$rid=\{request id}此端點，推送一JSON格式之訊息，其訊息如下。   
Ex:
 
```cs
-->
{
    "localIp(保留字)"   : "{區網IP}"(選用),
    "{tagKey1}" : "{tagValue1}"(選用),
    "{tagKey2}" : "{tagValue2}"(選用),
    ...
}
```
* 若資料是給**CoreService**讀取使用的，則請寫至**第一層**。   

* 其中tagValue若有多組比對項，則請以**逗點**分隔開。
Ex: A,B,C   

* 如果要將某個Tag刪除，則設定該項目之value為**null**即可。  

* 標籤、所需屬性和報告屬性中所有 JSON 物件的深度上限為 **5**。  
* 所有字串值的最大長度為 **512** 個位元組。

---   


#### 2.4、如何透過mqtt協定將訊息發送給特定裝置及接收訊息
　　目前提供兩個傳訊的方法，分別為一般**C2D**以及**直接方法**傳訊。其中直接方法必須要回覆訊息給發送者。  
　　若要發送訊息給特定裝置，則於.To屬性內夾帶一JSON，並且推送出所要發送之訊息。    

```cs
-->
//C2D傳訊
JObject jObject = new JObject()
{
    new JProperty("deviceId",{接收者的deviceId}") 
};
<--
透過監聽receiveMsg接點("devices/{deviceId}/messages/devicebound/#")即可收到訊息。

-->
//直接方法傳訊
JObject jObject = new JObject()
{
    new JProperty("directmethod",{接收者的deviceId}") 
};
<--
透過監聽Topic("$iothub/methods/POST/directMethod/#")即可收到訊息。
由於直接方法需要回覆，故請傳一則空的JSON至sendDirectMethod接點($iothub/methods/res/200/?$rid=)，
其中$rid的參數為收到該則訊息時系統所帶進來的。
```
**接收者的deviceId區分大小寫**。   
若一次要傳送給多個裝置，請以**逗點分隔**，如device1,device2。


---


#### 2.5、如何透過mqtt協定訂閱上下線通知
　　若要訂閱該群組內的上下線通知，則於.To屬性內夾帶一JSON，並且推送出一則空訊息。   
　　目前有兩種訂閱之狀態，分別為**訂閱該群組所有成員之狀態(all)**及**只訂閱該群組發起者(manager)**。   
Ex:    

```cs
-->
JObject jObject = new JObject()
{
    new JProperty("connectionState",{value})
    //當value為all時代表訂閱該群組所有的上下線狀態，為manager時則只訂閱該群組的發起者，若為false則代表取消訂閱上下線狀態。
};

      

若有裝置上下線，則會**收到**以下之JSON

```cs
<--
{
    connectionState
    {
        "true"  : ["deviceId1","deviceId2",...]" ,
        "false" : ["deviceId3","deviceId4",...]" 
    }
}
```
當Key為true時則代表上線，為false時則為下線，如果Value為空，則不顯示Key及Value。   
若該裝置自身網路連線品質不佳造成連續上下線，則該裝置可能會同時出現於上線與下線。   Ex:"true":["deviceId1"],"false":["deviceId1"]  

---

#### 2.6、如何透過mqtt協定發送一則公告訊息
　　若要發送一則公告訊息，則於.To屬性內夾帶一JSON，並且推送出所要公告之訊息，每個裝置限定只能發送**一則**公告訊息。    

```cs
-->
JObject jObject = new JObject()
{
    new JProperty("broadcast", true or false)//(不區分大小寫)當值為true時則記錄此則訊息，若為false則刪除所紀錄之訊息
};
```
訊息推送出去後，之後加入該群組之裝置一旦**上線**，即會收到公告訊息。   
如要**覆蓋**之前的公告訊息，則value帶入true並直接推送，即可覆蓋舊公告訊息。

---

#### 2.7、如何透過mqtt協定發送保留訊息以及取得保留訊息
　　若要保留訊息，則於傳送訊息的.to屬性內多增加一關鍵字"keepMessage"(key)及訊息編號，此關鍵字必須於JSON的第一筆。   
　　此動作會將訊息傳給裝置，並且將該訊息及該收件人紀錄至Table內。

```cs
-->
JObject jObject = new JObject()
{
    new JProperty("keepMessage","{訊息編號}"),//(keepMessage不區分大小寫，訊息編號請以半形數字做紀錄)
    new JProperty("directmethod",{接收者的deviceId}") (舉例)
};
```    
      
　　若要取得保留訊息，則於.To屬性內夾帶一Key為"getkeepmessage"之關鍵字，value為要從第幾則訊息開始抓取，例如要抓取第五則以後的訊息，且包含第五則，則於value內輸入5即可。

```cs
-->
JObject jObject = new JObject()
{
    new JProperty("getkeepMessage","{訊息編號}")//(getkeepMessage不區分大小寫)
};
```
 
```cs
<--
{
    {訊息編號}:{訊息},
    {訊息編號}:{訊息},
    ...
}
```


---


#### 2.8、如何透過Azure Storage Client Library上傳或下載檔案(Blob)
　　目前Azure Storage Client Library支援.NET、Java及IOS。檔案上傳完後請發送完成訊息至群組，其參數請依照下面範例填寫。   

上傳檔案:   
```cs
.Net及Java
CloudBlobClient blobClient = new CloudBlobClient(new Uri({blobContainerSasUri}));
CloudBlobContainer container = blobClient.GetContainerReference("{GroupNum}");  
CloudBlockBlob blockBlob = container.GetBlockBlobReference("{FileName}");
using (var fileStream = File.OpenRead({filePath:欲上傳的檔案路徑}))
{
    blockBlob.UploadFromStream(fileStream);
}

IOS(此部分尚未測試過)
AZSCloudBlobClient *blobClient = [account getBlobClient];
AZSCloudBlobContainer *blobContainer = [blobClient containerReferenceFromName:@"{GroupNum}"];   
AZSCloudBlockBlob *blockBlob = [blobContainer blockBlobReferenceFromName:@"{FileName}"];
```

於Blob內的存放狀況會是GroupNum\FileName。


下載檔案:   
```cs
CloudBlobClient blobClient = new CloudBlobClient(new Uri({blobContainerSasUri}));
CloudBlobContainer container = blobClient.GetContainerReference("{GroupNum}");  
CloudBlockBlob blockBlob = container.GetBlockBlobReference("{FileName}");
using (var fileStream = File.OpenWrite({filePath:下載下來的檔案放置路徑}))
{
    blockBlob.DownloadToStream(fileStream);
}
```

---

#### 2.9、如何透過mqtt協定定期發送裝置心跳   
**由各裝置實作**。  
　　mqtt連線後並閒置一段時間會自動斷開連線，有鑑於此，希望各裝置定時傳送一訊息至Azure IoT Hub內，此訊息只會進行單向傳輸，故只會傳送至Azure IoT Hub內並不會再做其他邏輯應用。
```cs
-->
JObject jObject = new JObject()
{
    new JProperty("HeartBeat","")//(HeartBeat不區分大小寫)
};
```
　　目前測試是閒置十分鐘後即會自動斷線，故若是在十分鐘內都無傳送訊息則必須發送一則心跳訊息至Azure。

---

### 3、如何結束服務  
#### 3.1、結束服務  
**由服務發起者呼叫**。   

　　呼叫此函式會刪除群組及Blob內之檔案。   
　　若要結束服務，則於傳送訊息的.to屬性夾帶一關鍵字"DeleteGroup"(key)，即可刪除群組。

```cs
-->
JObject jObject = new JObject()
{
    new JProperty("DeleteGroup","")
};
```    
