const express = require("express");
const router = express.Router();

const subscriberController = require("../controllers/subscriber");

// Subscriber Home Route.
router.get("/", subscriberController.getSubscriberPage);

module.exports = router;