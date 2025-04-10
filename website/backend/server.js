const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const mqtt = require("mqtt");
const cors = require("cors");

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: "*", // Update with your frontend URL in production
    methods: ["GET", "POST"],
  },
});

// Enable CORS for Express API
app.use(cors());
app.use(express.json());

// MQTT Setup
const MQTT_BROKER_URL = "mqtt://test.mosquitto.org"; // Or your local broker
const MQTT_TOPIC = "gpslocation"; // Must match your ESP32 topic
const MQTT_TOPIC2 = "esp32cam/image";

const mqttClient = mqtt.connect(MQTT_BROKER_URL);

mqttClient.on("connect", () => {
  console.log("✅ MQTT connected");

  // subscribe to location topic
  mqttClient.subscribe(MQTT_TOPIC, (err) => {
    if (!err) {
      console.log(`📡 Subscribed to topic: ${MQTT_TOPIC}`);
    } else {
      console.error("Failed to subscribe:", err);
    }
  });
  // subscribe to image topic
  mqttClient.subscribe(MQTT_TOPIC2, (err) => {
    if (!err) {
      console.log(`📡 Subscribed to topic: ${MQTT_TOPIC2}`);
    } else {
      console.error("Failed to subscribe:", err);
    }
  });
});

// Handle messages from both topics
mqttClient.on("message", (topic, message) => {
  try {
    if (topic === MQTT_TOPIC) {
      const data = JSON.parse(message.toString());
      const { lat, lng } = data;

      if (lat && lng) {
        io.emit("locationUpdate", { latitude: lat, longitude: lng });
        console.log("📍 Location emitted:", lat, lng);
      }
    } else if (topic === MQTT_TOPIC2) {
      const base64Image = message.toString(); // raw base64 string from ESP32-CAM
      if (base64Image) {
        io.emit("imageUpdate", { imageBase64: base64Image });
        console.log("🖼️ Image emitted");
      }
    }
  } catch (err) {
    console.error("❌ Failed to handle MQTT message:", err.message);
  }
});

// Socket.IO Connection
io.on("connection", (socket) => {
  console.log("🧩 A client connected:", socket.id);

  socket.on("disconnect", () => {
    console.log("❌ Client disconnected:", socket.id);
  });
});

// Optional root route
app.get("/", (req, res) => {
  res.send("🔧 Backend is running");
});

// Start server
const PORT = 5000;
server.listen(PORT, () => {
  console.log(`🚀 Server listening on http://localhost:${PORT}`);
});
