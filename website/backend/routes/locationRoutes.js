// routes/locationRoutes.js
const express = require('express');
const Location = require('../models/Location');
const router = express.Router();

module.exports = (io) => {
  router.post('/', async (req, res) => {
    const { userId, latitude, longitude } = req.body;
    const location = new Location({ userId, latitude, longitude });
    await location.save();
    io.emit('locationUpdate', location);
    res.status(201).json(location);
  });
  return router;
};
