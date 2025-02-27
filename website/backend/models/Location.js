// models/Location.js
const mongoose = require('mongoose');
const LocationSchema = new mongoose.Schema({
  userId: String,
  latitude: Number,
  longitude: Number,
  timestamp: { type: Date, default: Date.now }
});
module.exports = mongoose.model('Location', LocationSchema);