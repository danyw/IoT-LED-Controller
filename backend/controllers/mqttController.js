const mqttConfig = require("../config/mqttConfig");

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