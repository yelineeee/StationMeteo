const mqtt = require("mqtt");
const WebSocket = require("ws");

const MQTT_URL = "mqtt://captain.dev0.pandor.cloud:1884";
const MQTT_TOPIC = "station/temperature";

const WSS_PORT = 8080;

const mqttClient = mqtt.connect(MQTT_URL);

const wss = new WebSocket.Server({ port: WSS_PORT });

mqttClient.on("connect", () => {
  console.log("Connected to broker MQTT");
  mqttClient.subscribe(MQTT_TOPIC);});

mqttClient.on("message", (topic, message) => {
  const data = message.toString();
  console.log("MQTT →", data);
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(data);
    }
  });
});

wss.on("connection", ws => {
  console.log("socket co");
});

console.log(`socket dispo sur ws://localhost:${WSS_PORT}`);
