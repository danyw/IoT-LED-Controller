const mongoose = require('mongoose');

const ledProfileSchema = new mongoose.Schema({
  name: {
    type: String,
    required: true,
  },
  model: {
    type: String,
    unique: true,
    required: true,
  },
  color: {
    type: String,
    required: true,
  },
  wattage: {
    type: number,
    required: true,
  },
  spectrum: [
    {
      wavelength: { type: Number, required: true },
      intensity: { type: Number, required: true },
    },
  ],
});

module.exports = mongoose.model('LedProfile', ledProfileSchema);
