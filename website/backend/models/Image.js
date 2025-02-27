// models/Image.js
const mongoose = require('mongoose');

const ImageSchema = new mongoose.Schema({
    userId: String,
    imageUrl: String,
    timestamp: { type: Date, default: Date.now }
  });
  module.exports = mongoose.model('Image', ImageSchema);
  