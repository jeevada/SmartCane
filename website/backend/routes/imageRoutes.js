// routes/imageRoutes.js
const express = require('express');
const multer = require('multer');
const path = require('path');
const Image = require('../models/Image');

const router = express.Router();
const storage = multer.diskStorage({
  destination: './uploads/',
  filename: (req, file, cb) => {
    cb(null, Date.now() + path.extname(file.originalname));
  }
});
const upload = multer({ storage });

module.exports = (io) => {
  router.post('/', upload.single('image'), async (req, res) => {
    const { userId } = req.body;
    const imageUrl = `${req.protocol}://${req.get('host')}/uploads/${req.file.filename}`;
    const image = new Image({ userId, imageUrl });
    await image.save();
    io.emit('imageUpdate', image);
    res.status(201).json(image);
  });
  return router;
};