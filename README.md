# AQMS
## Air Quality Monitoring System

The system is about Air Quality Monitoring System in which the sensors detect temperature, humidity, and gas changes (either increases or decreases.)

The following code from Apps scripts connects my IoT system to Google sheets.

```cpp
function doGet(e) {
  // Parse parameters from the URL query string
  var temperature = parseFloat(e.parameter.temperature);
  var temperatureStatus = e.parameter.temperatureStatus; 
  var humidity = parseFloat(e.parameter.humidity);
  var humidityStatus = e.parameter.humidityStatus;
  var gasLevel = parseFloat(e.parameter.gasLevel);
  var gasLevelStatus = e.parameter.gasLevelStatus;
  var spreadsheetId = '10-OeuvMbjfpTcOjWToNdslOnz9QbNM5lNsH1lflLYJw'; // Replace with your actual spreadsheet ID
  
  // Format date and time using Utilities class
  var today = new Date();
  var formattedDate = Utilities.formatDate(today, Session.getScriptTimeZone(), 'yyyy-MM-dd');
  var formattedTime = Utilities.formatDate(today, Session.getScriptTimeZone(), 'HH:mm:ss');

  var ss = SpreadsheetApp.openById(spreadsheetId);
  var sheet = ss.getSheetByName('Sheet1'); // Replace 'Sheet1' with your actual sheet name
  
  // Append new row with data
  sheet.appendRow([formattedDate, formattedTime, temperature, temperatureStatus, humidity, humidityStatus, gasLevel, gasLevelStatus]);
  
  // Return JSON response (optional)
  var response = {
    status: 'SUCCESS',
    message: 'Record added successfully'
  };
  
  return ContentService.createTextOutput(JSON.stringify(response))
    .setMimeType(ContentService.MimeType.JSON);
}
```
