const express = require('express');
const router = express.Router();
const mqttController = require('../controllers/mqttController');

router.post('/publish', mqttController.publishMessage);
router.post('/subscribe', mqttController.subscribeToTopic);

module.exports = router;
