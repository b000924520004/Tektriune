# CoreServiceAzure
* [CoreAPI](#CoreAPI)
  * [公開API](#公開API)
    * [裝置註冊](#裝置註冊)
  * [私有API](#私有API)  
## CoreAPI 

### 公開API
---
**裝置註冊**

  ```cs 
  public async Task<Access> Regist(string deviceName, string clientId)
  ```
  >幫用戶取得驗證碼及Token；首先先去金鑰庫取得當日的UUID，然後再取得此APP的金鑰，並使用其金鑰取得進入需要Authorize API的Token最後再進行裝置的註冊。

`deviceName` 該裝置之名稱

`clientId` 應用程式識別碼

---