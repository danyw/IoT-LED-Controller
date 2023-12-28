const express = require("express");
const router = express.Router();

const publisherController = require("../controllers/publisher");


router.post("/", publisherController.publishMQTTMessage);

module.exports = router;