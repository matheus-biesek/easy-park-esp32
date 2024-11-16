#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17

void connectWifi() {
  const char* ssid = "BIESEK";
  const char* password = "matheuS123";

  Serial.println("Conectando ao WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Tentando conectar...");
  }
  Serial.println("Conectado ao WiFi");
}

class StatusManager {
  private:
  bool messageAdmAvailable = false;
  bool gateOneStatusAvailable = false;
  bool vacancyOneStatusAvailable = false;
  bool vacancyTwoStatusAvailable = false;
  bool vacancyOneHistoryAvailable = false;
  bool vacancyTwoHistoryAvailable = false;

  public:
  bool isVacancyOneHistoryAvailable() {
    return vacancyOneHistoryAvailable;
  }
  
  bool isVacancyTwoHistoryAvailable() {
    return vacancyTwoHistoryAvailable;
  }

  bool isMessageAdmAvailable() {
    return messageAdmAvailable;
  }

  bool isGateOneStatusAvailable() {
    return gateOneStatusAvailable;
  }

  bool isVacancyOneStatusAvailable() {
    return vacancyOneStatusAvailable;
  }

  bool isVacancyTwoStatusAvailable() {
    return vacancyTwoStatusAvailable;
  }

  void setMessageAdmAvailable(bool status) {
    messageAdmAvailable = status;
  }

  void setGateOneStatusAvailable(bool status) {
    gateOneStatusAvailable = status;
  }

  void setVacancyOneStatusAvailable(bool status) {
    vacancyOneStatusAvailable = status;
  }

  void setVacancyTwoStatusAvailable(bool status) {
    vacancyTwoStatusAvailable = status;
  }

  void setVacancyOneHistoryAvailable(bool status) {
    vacancyOneHistoryAvailable = status;
  }
  
  void setVacancyTwoHistoryAvailable(bool status) {
    vacancyTwoHistoryAvailable = status;
  }
};

class Serial2Data {
  private:
  bool vacancyOneStatus;
  bool vacancyTwoStatus;

  public:
  Serial2Data() : vacancyOneStatus(false), vacancyTwoStatus(false) {}

  bool getVacancyOneStatus() {
    return vacancyOneStatus;
  }

  bool getVacancyTwoStatus() {
    return vacancyTwoStatus;
  }

  void setVacancyOneStatus(bool status) {
    vacancyOneStatus = status;
  }

  void setVacancyTwoStatus(bool status) {
    vacancyTwoStatus = status;
  }

  void processReceivedData(StatusManager& statusManager) {
    if (Serial2.available() > 0) {
      String receivedData = Serial2.readStringUntil('\n');
      Serial.println("Dados recebidos: " + receivedData);
      if (receivedData.startsWith("vacancyOne:")) {
          String statusOne = receivedData.substring(receivedData.indexOf(':') + 1);
          statusOne.trim();
          if (statusOne == "true") {
            setVacancyOneStatus(true);
            statusManager.setVacancyOneStatusAvailable(true);
          } else if (statusOne == "false") {
            setVacancyOneStatus(false);
            statusManager.setVacancyOneStatusAvailable(true);
          }
      } else if (receivedData.startsWith("vacancyTwo:")) {
          String statusTwo = receivedData.substring(receivedData.indexOf(':') + 1);
          statusTwo.trim();
          if (statusTwo == "true") {
            setVacancyTwoStatus(true);
            statusManager.setVacancyTwoStatusAvailable(true);
          } else if (statusTwo == "false") {
            setVacancyTwoStatus(false);
            statusManager.setVacancyTwoStatusAvailable(true);
          }
      } else {
        Serial.println("Dado enviado pelo Arduino não corresponde a uma variável local! Dado recebido: " + String(receivedData));
      }
    }
  }
};

class ApiHandler {
  private:
  HTTPClient http;
  String authToken;
  String urlChangeVacancyStatus;
  String urlGateOneStatus;
  String urlMessageAdm;
  String urlSaveVacancyHistory;

  String apiResultMessageAdm;
  String apiResultGateOneStatus;
  String apiResultChangeVacancyStatus;

  public:
  ApiHandler(String token, String changeVacancyUrl, String gateOneStatusUrl, String messageAdmUrl, String vacancyHistoricUrl){
    authToken = token;
    urlChangeVacancyStatus = changeVacancyUrl;
    urlGateOneStatus = gateOneStatusUrl;
    urlMessageAdm = messageAdmUrl;
    urlSaveVacancyHistory = vacancyHistoricUrl;
    apiResultMessageAdm = "";
    apiResultGateOneStatus = "";
    apiResultChangeVacancyStatus = "";
  }

  String getApiResultMessageAdm() {
    return apiResultMessageAdm;
  }

  void setApiResultMessageAdm(const String& result) {
    apiResultMessageAdm = result;
  }

  String getApiResultGateOneStatus() {
    return apiResultGateOneStatus;
  }

  void setApiResultGateOneStatus(const String& result) {
    apiResultGateOneStatus = result;
  }

  String getApiResultChangeVacancyStatus() {
    return apiResultChangeVacancyStatus;
  }

  void setApiResultChangeVacancyStatus(const String& result) {
    apiResultChangeVacancyStatus = result;
  }

  String getUrlChangeVacancyStatus() {
    return urlChangeVacancyStatus;
  }

  String getUrlSaveVacancyHistory() {
    return urlSaveVacancyHistory;
  }

  void setUrlChangeVacancyStatus(const String& url) {
    urlChangeVacancyStatus = url;
  }

  String getUrlGateOneStatus() {
    return urlGateOneStatus;
  }

  void setUrlGateOneStatus(const String& url) {
    urlGateOneStatus = url;
  }

  String getUrlMessageAdm() {
    return urlMessageAdm;
  }

  void setUrlMessageAdm(const String& url) {
    urlMessageAdm = url;
  }

  String makeHttpRequestGet(const String& url) {
    if (WiFi.status() == WL_CONNECTED) {
      http.begin(url);
      http.addHeader("Authorization", "Bearer " + authToken);
      int responseCode = http.GET();
      if (responseCode > 0) {
        String response = http.getString();
        http.end();
        return response;
      } else {
        Serial.println("Erro na requisição GET: " + http.errorToString(responseCode));
        http.end();
        return "";
      }
    } else {
      Serial.println("Erro: Não conectado ao WiFi");
      return "";
    }
  }

  String makeHttpRequestPost(const String& postData, const String& url) {
    if (WiFi.status() == WL_CONNECTED) {
      http.begin(url);
      http.addHeader("Authorization", "Bearer " + authToken);
      http.addHeader("Content-Type", "application/json");
      int responseCode = http.POST(postData);
      if (responseCode > 0) {
        String response = http.getString();
        http.end();
        return String(response);
      } else {
        Serial.println("Erro na requisição POST: " + http.errorToString(responseCode));
        http.end();
        return "";
      }
    } else {
      Serial.println("Erro: Não conectado ao WiFi");
      return "";
    }
  }

  String makeHttpRequestPut(const String& postData, const String& url) {
    if (WiFi.status() == WL_CONNECTED) {
      http.begin(url);
      http.addHeader("Authorization", "Bearer " + authToken);
      http.addHeader("Content-Type", "application/json");
      int responseCode = http.PUT(postData);
      if (responseCode > 0) {
        String response = http.getString();
        http.end();
        return response;
      } else {
        Serial.println("Erro na requisição PUT: " + http.errorToString(responseCode));
        http.end();
        return "";
      }
    } else {
      Serial.println("Erro: Não conectado ao WiFi");
      return "";
    }
  }
};

StatusManager statusManager;

Serial2Data serial2Data;

ApiHandler apiEasyPark(
  "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJsb2dpbi1hdXRoLWlwYSIsInN1YiI6ImFkbXIiLCJyb2xlIjowLCJleHAiOjE3MzE0NzMxNjd9.A1DgXIMXFXWVhRz38NYHuqAkTtCrUwHCk8hPMrpGdwA", 
  "https://www.easypark.services/vacancy/change-vacancies-status", 
  "https://www.easypark.services/parking-lot/get-gate-status", 
  "https://www.easypark.services/parking-lot/get-message-adm",
  "https://www.easypark.services/vacancy/save-vacancy-history"
);

// Core 1
void requestAPI(void *pvParameters) {
  String apiResult;

  while (true) {
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
    
    String postGateOne = "{\"gate\":\"ONE\"}";
    String postVacancyOneStatus = "[{\"position\":" + String(1) + ",\"status\":\"" + (serial2Data.getVacancyOneStatus() ? "busy" : "available") + "\"}]";
    String postVacancyTwoStatus = "[{\"position\":" + String(2) + ",\"status\":\"" + (serial2Data.getVacancyTwoStatus() ? "busy" : "available") + "\"}]";
    String postVacancyOneHistory = "{\"position\":" + String(1) + ",\"status\":\"" + (serial2Data.getVacancyOneStatus() ? "busy" : "available") + "\"}";
    String postVacancyTwoHistory = "{\"position\":" + String(2) + ",\"status\":\"" + (serial2Data.getVacancyTwoStatus() ? "busy" : "available") + "\"}";

    apiResult = apiEasyPark.makeHttpRequestGet(apiEasyPark.getUrlMessageAdm());
    if (apiResult != ""){
      apiEasyPark.setApiResultMessageAdm(apiResult);  
      statusManager.setMessageAdmAvailable(true);
      apiResult = "";
    }

    apiResult = apiEasyPark.makeHttpRequestPost(postGateOne,apiEasyPark.getUrlGateOneStatus());
    if (apiResult != ""){
      apiEasyPark.setApiResultGateOneStatus(apiResult);
      statusManager.setGateOneStatusAvailable(true);
      apiResult = "";
    }
    
    if (statusManager.isVacancyOneStatusAvailable()){
      apiResult = apiEasyPark.makeHttpRequestPut(postVacancyOneStatus, apiEasyPark.getUrlChangeVacancyStatus()); 
      if (apiResult != ""){
        statusManager.setVacancyOneStatusAvailable(true);
        apiResult = "";  
      }
    }

    if (statusManager.isVacancyTwoStatusAvailable()){
      apiResult = apiEasyPark.makeHttpRequestPut(postVacancyTwoStatus, apiEasyPark.getUrlChangeVacancyStatus());
      if (apiResult != ""){
        statusManager.setVacancyTwoStatusAvailable(true);  
        apiResult = "";
      }
    }

    if (statusManager.isVacancyOneHistoryAvailable()){
      apiResult = apiEasyPark.makeHttpRequestPost(postVacancyOneHistory, apiEasyPark.getUrlSaveVacancyHistory());
      if (apiResult =! ""){
        statusManager.setVacancyOneHistoryAvailable(false);
        apiResult = "";
      }
    }

    if (statusManager.isVacancyTwoHistoryAvailable()){
      apiResult = apiEasyPark.makeHttpRequestPost(postVacancyTwoHistory, apiEasyPark.getUrlSaveVacancyHistory());
      if (apiResult != ""){
        statusManager.setVacancyTwoHistoryAvailable(false);
        apiResult = "";
      }
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

// Core 0
void handleSerial(void *pvParameters) {
  String messageAdmFormated = "";
  String gateOneStatusFormated = "";
  bool lastVacancyOneHistoric;
  bool lastVacancyTwoHistoric;

  while (true) {
    serial2Data.processReceivedData(statusManager);

    if (serial2Data.getVacancyOneStatus() != lastVacancyOneHistoric){
      lastVacancyOneHistoric = serial2Data.getVacancyOneStatus();
      statusManager.setVacancyOneHistoryAvailable(true);
    }

    if (serial2Data.getVacancyTwoStatus() != lastVacancyTwoHistoric){
      lastVacancyTwoHistoric = serial2Data.getVacancyTwoStatus();
      statusManager.setVacancyTwoHistoryAvailable(true);
    }

    if (statusManager.isMessageAdmAvailable()) {
      statusManager.setMessageAdmAvailable(false);
      messageAdmFormated = "messageAdm:" + apiEasyPark.getApiResultMessageAdm();
      messageAdmFormated.trim();
      Serial2.println(messageAdmFormated);
      Serial.println("Mensagem enviada para o arduino - " + messageAdmFormated);
    }
    
    if (statusManager.isGateOneStatusAvailable()) {
      statusManager.setGateOneStatusAvailable(false);
      gateOneStatusFormated = "gateOneStatus:" + apiEasyPark.getApiResultGateOneStatus();
      gateOneStatusFormated.trim();
      Serial2.println(gateOneStatusFormated);
      Serial.println("Mensagem enviada para o arduino - " + gateOneStatusFormated);
    }
    vTaskDelay(125 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  connectWifi();

 // Core 1
  xTaskCreatePinnedToCore(requestAPI, "RequestAPI", 10000, NULL, 1, NULL, 1);

  // Core 0
  xTaskCreatePinnedToCore(handleSerial, "HandleSerial", 10000, NULL, 1, NULL, 0);
}

void loop() {}
