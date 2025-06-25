import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import './Navbar.css';

const Navbar = () => {
  const [batteryLevel, setBatteryLevel] = useState(null);

  useEffect(() => {
    fetch('/api/clima')
      .then((res) => res.json())
      .then((data) => {
        if (data.success && data.data.Bateria) {
          setBatteryLevel(data.data.Bateria.value);
        }
      })
      .catch((error) => {
        console.error('Erro ao obter nível da bateria:', error);
      });
  }, []);

  const batteryPercent = batteryLevel !== null ? Math.round(batteryLevel) : '--';

  return (
    <nav className="navbar">
      <div className="navbar-left">
        <div className="navbar-title">Smart Weather App</div>
        <div className="battery-container">
          <div className="battery">
            <div
              className="battery-level"
              style={{
                height: `${batteryPercent}%`,
                backgroundColor:
                  batteryPercent > 50
                    ? '#2ecc71'
                    : batteryPercent > 20
                    ? '#f1c40f'
                    : '#e74c3c',
              }}
            />
          </div>
          <span className="battery-text">{batteryPercent}%</span>
        </div>
      </div>
      <ul className="navbar-links">
        <li><Link to="/">Início</Link></li>
        <li><Link to="/histórico">Histórico</Link></li>
        <li><Link to="/sobre">Sobre</Link></li>
      </ul>
    </nav>
  );
};

export default Navbar;