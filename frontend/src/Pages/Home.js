import React, { useEffect, useState } from 'react';
import axios from 'axios';
import ClimaBoxes from '../Components/ClimaBoxes';
import Mapa from '../Components/Mapa';
import './App.css';

const Home = () => {
  const [dadosClima, setDadosClima] = useState(null);
  const [loading, setLoading] = useState(true);
  const [erro, setErro] = useState(null);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const res = await axios.get('/api/clima');
        if (res.data.success) {
          setDadosClima(res.data.data);
          setErro(null);
        } else {
          setErro(res.data.message || 'Erro ao obter dados');
        }
      } catch (err) {
        console.error('Erro:', err);
        setErro('Erro ao carregar dados climáticos');
      } finally {
        setLoading(false);
      }
    };

    fetchData();
    const interval = setInterval(fetchData, 30000);
    return () => clearInterval(interval);
  }, []);

  return (
    <main className="home-container">
      <div className="home-grid">
        <div className="mapa-wrapper">
          <Mapa />
        </div>
        <div className="clima-wrapper">
          <ClimaBoxes dadosClima={dadosClima} loading={loading} erro={erro} />
        </div>
      </div>
    </main>
  );
};

export default Home;
