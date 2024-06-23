const express = require('express');
const router = express.Router();
const ledProfileController = require('../controllers/ledProfileContoller');

router
  .route('/')
  .get(ledProfileController.getAllLedProfiles)
  .post(ledProfileController.createNewLedProfile)
  .patch(ledProfileController.updateLedProfile)
  .delete(ledProfileController.deleteLedProfile);

module.exports = router;
