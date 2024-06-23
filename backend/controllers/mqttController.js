const mqttService = require('../middleware/mqttService');

const MQTT_HOST = 'mqtt://localhost:1883';
var mqttClient = new mqttService(MQTT_HOST);
mqttClient.connect();

// Publish MQTT Message
exports.publishMessage = async (req, res) => {
  try {
    const { topic, message } = req.body;
    mqttClient.publish(topic, message);
    res.json({ message: 'Message published successfully' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
};

// Subscribe to MQTT Topic
exports.subscribeToTopic = async (req, res) => {
  try {
    const { topic } = req.body;
    mqttClient.subscribe(topic);
    res.json({ message: `Subscribed to ${topic}` });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
};

// Publish MQTT Message - LED channel value 
exports.ledControl = async (req, res) => {
  try {
    const { value, color } = req.body;
console.log(value);
console.log(color);
    const colorChannelMap = {
      'warmwhite': 1,
      'coolwhite': 5,
      'green': 3,
      'blue': 4,
      'red': 2
    };

    const channel = colorChannelMap[color.toLowerCase()];
    console.log(channel);

    if (value < 0 || value > 1000) {
      return res.status(400).json({ message: "Invalid brightness value" });
    }

    const scaledValue = Math.round((value / 1000) * 254);

    const brightnessKey = `brightness_button_${channel}`;
    const mqttMessage = JSON.stringify({ [brightnessKey]: scaledValue });
    const topic = "zigbee2mqtt/0x0080e12500e0a521/set";

    mqttClient.publish(topic, mqttMessage);
    res.status(200).json({ message: 'LED control message published successfully' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
}; 

