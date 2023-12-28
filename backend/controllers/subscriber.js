const mqttService = require("../middleware/mqttService");

exports.subscribeToTopic = async (req, res) => {
    try {
      const topic = req.body.topic;
      const message = req.body.message;

      mqttClient.subscribe(topic);
      res.json({ message: `Subscribed to ${topic}` });
    } catch (error) {
      res.status(500).json({ error: error.message });
    }
  };