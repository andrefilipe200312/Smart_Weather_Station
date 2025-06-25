import React, { useState } from 'react';
import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import './Mapa.css';

import L from 'leaflet';
import iconUrl from 'leaflet/dist/images/marker-icon.png';
import iconShadowUrl from 'leaflet/dist/images/marker-shadow.png';

let DefaultIcon = L.icon({
  iconUrl,
  shadowUrl: iconShadowUrl,
  iconAnchor: [12, 41],
});

L.Marker.prototype.options.icon = DefaultIcon;

const Mapa = () => {
  const position = [38.69095588380319, -9.299971930569946];
  const [dadosClima, setDadosClima] = useState(null);
  const [carregando, setCarregando] = useState(false);
  const [erro, setErro] = useState(null);

  const buscarDadosClima = async () => {
    setCarregando(true);
    setErro(null);
    try {
      const resposta = await fetch("/api/clima");
      const json = await resposta.json();
      if (json.success) {
        setDadosClima(json.data);
      } else {
        setErro("Erro ao obter dados");
      }
    } catch (e) {
      setErro("Erro de rede");
    } finally {
      setCarregando(false);
    }
  };

  return (
    <div className="mapa-container">
      <MapContainer center={position} zoom={13} style={{ height: '100%', width: '100%' }}>
        <TileLayer
          attribution='&copy; <a href="https://osm.org/copyright">OpenStreetMap</a>'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <Marker position={position}>
          <Popup eventHandlers={{ add: buscarDadosClima }}>
            <div style={{ minWidth: '200px' }}>
              <h4>Últimos Dados Climáticos</h4>
              {carregando && <p>Carregando...</p>}
              {erro && <p style={{ color: 'red' }}>{erro}</p>}
              {dadosClima && (
                <ul style={{ listStyle: 'none', padding: 0 }}>
                  {Object.entries(dadosClima).map(([key, val]) => (
                    <li key={key}>
                      <strong>{key}:</strong> {val.value} ({new Date(val.time).toLocaleString()})
                    </li>
                  ))}
                </ul>
              )}
              {!carregando && !erro && !dadosClima && <p>Nenhum dado disponível.</p>}
            </div>
          </Popup>
        </Marker>
      </MapContainer>
    </div>
  );
};

export default Mapa;
