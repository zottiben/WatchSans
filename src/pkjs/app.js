Pebble.addEventListener("ready", function(e) {
  console.log("PebbleKit JS Ready!");
});

Pebble.addEventListener("showConfiguration", function(e) {
  var url = "http://www.benzotti.co/pebbledev/WatchSans/watchsans.html";
  
  console.log('Showing Settings Page: ' + url);
  
  Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
  var configuration = JSON.parse(decodeURIComponent(e.response));
  
  console.log("Configuration window returned: " + JSON.stringify(configuration));
  
  if(configuration.textColor) {
    Pebble.sendAppMessage({
      "textColor" : parseInt(configuration.textColor, 16),
      "backgroundColor" : parseInt(configuration.backgroundColor, 16),
      "isMeridian": configuration.isMeridian
    },
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    ); 
  }
});