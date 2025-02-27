// server.js (Main Entry Point)
const express = require('express');
const cors = require('cors');
const http = require('http');
const { Server } = require('socket.io');
const mongoose = require('mongoose');
const locationRoutes = require('./routes/locationRoutes');
const imageRoutes = require('./routes/imageRoutes');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: '*',
    methods: ['GET', 'POST']
  }
});

// Middleware
app.use(cors());
app.use(express.json());
app.use('/uploads', express.static(path.join(__dirname, 'uploads')));

// MongoDB Connection
mongoose.connect('mongodb://localhost:27017/smartcane', {
  useNewUrlParser: true,
  useUnifiedTopology: true
});

// WebSocket Connection
io.on('connection', (socket) => {
  console.log('New client connected');
  socket.on('disconnect', () => console.log('Client disconnected'));
});

// Routes
app.use('/location', locationRoutes(io));
app.use('/image', imageRoutes(io));

const PORT = 5000;
server.listen(PORT, () => console.log(`Server running on port ${PORT}`));