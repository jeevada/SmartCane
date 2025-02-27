import React, { useState, useEffect } from "react";
import io from "socket.io-client";
import { MapContainer, TileLayer, Marker, Popup } from "react-leaflet";
import "leaflet/dist/leaflet.css";
import L from "leaflet";
import logo from "../assets/logo.webp";

// Fix for missing marker icons in Leaflet
import markerIconPng from "leaflet/dist/images/marker-icon.png";
import markerShadowPng from "leaflet/dist/images/marker-shadow.png";
const defaultIcon = L.icon({
  iconUrl: markerIconPng,
  shadowUrl: markerShadowPng,
  iconSize: [25, 41],
  iconAnchor: [12, 41],
});

const socket = io("http://localhost:5000");

const Dashboard = () => {
  const [location, setLocation] = useState(null);
  const [imageUrl, setImageUrl] = useState(null);

  useEffect(() => {
    socket.on("locationUpdate", (data) => {
      setLocation(data);
    });

    socket.on("imageUpdate", (data) => {
      setImageUrl(data.imageUrl);
    });

    return () => {
      socket.off("locationUpdate");
      socket.off("imageUpdate");
    };
  }, []);

  return (
    <div className="min-h-screen bg-gray-100 p-6 flex flex-col items-center text-center">
      {/* Logo */}
      <img
        src={logo}
        alt="Blind Vision Logo"
        className="w-32 h-32 rounded-full mb-4 shadow-lg border-4 border-white"
      />
      <h1 className="text-3xl font-bold text-blue-600 mb-6">
        Blind Vision Dashboard
      </h1>

      {/* Map Section */}
      <div className="w-full max-w-2xl h-96 bg-white rounded-lg shadow-md overflow-hidden mb-6">
        {location ? (
          <MapContainer
            center={[location.latitude, location.longitude]}
            zoom={15}
            className="w-full h-full"
          >
            <TileLayer
              url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            />
            <Marker
              position={[location.latitude, location.longitude]}
              icon={defaultIcon}
            >
              <Popup>Blind Person's Location</Popup>
            </Marker>
          </MapContainer>
        ) : (
          <p className="text-gray-600 p-4">Waiting for location data...</p>
        )}
      </div>

      {/* Image Section */}
      <div className="w-full max-w-2xl bg-white rounded-lg shadow-md p-4">
        {imageUrl ? (
          <img
            src={imageUrl}
            alt="Captured Scene"
            className="w-full h-150 object-cover rounded-lg"
          />
        ) : (
          <p className="text-gray-600">Waiting for image...</p>
        )}
      </div>
    </div>
  );
};

export default Dashboard;
