import React, { useEffect, useState } from "react";
import axios from "axios";
import dayjs from "dayjs";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  Legend,
  ResponsiveContainer,
  CartesianGrid
} from "recharts";
import "./Dados_meteo.css";

const DadosMeteo = () => {
  const [dados, setDados] = useState({});
  const [loading, setLoading] = useState(true);
  const [range, setRange] = useState("-6h");

  useEffect(() => {
    const fetchData = async () => {
      try {
        setLoading(true);
        const res = await axios.get(`http://localhost:5000/api/clima/aggregated?range=${range}`);
        setDados(res.data.data || {});
      } catch (error) {
        console.error("Erro ao buscar dados:", error);
      } finally {
        setLoading(false);
      }
    };

    fetchData();
  }, [range]);

  const formatTick = (value) => {
    if (!value) return "";
    const date = new Date(value);
    if (["-1h", "-6h", "-12h", "-24h"].includes(range)) {
      return dayjs(date).format("HH:mm");
    } else {
      return dayjs(date).format("DD/MM HH:mm");
    }
  };

  const renderChart = (field, cor) => {
    const chartData = (dados[field] || []).map(item => ({
      time: item.time,
      value: item.value ?? null, // mantem os nulls
    }));

    return (
      <div className="grafico-container" key={field}>
        <h2 className="grafico-titulo">{field}</h2>
        <ResponsiveContainer width="100%" height={300}>
          <LineChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis
              dataKey="time"
              tickFormatter={formatTick}
              minTickGap={30}
            />
            <YAxis />
            <Tooltip
              labelFormatter={(value) =>
                dayjs(value).format("DD/MM/YYYY HH:mm")
              }
            />
            <Legend />
            <Line
              type="monotone"
              dataKey="value"
              stroke={cor}
              dot={false}
              connectNulls={true}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
    );
  };

  const campos = [
    { nome: "Temperatura", cor: "#f97316" },
    { nome: "Humidade", cor: "#3b82f6" },
    { nome: "Pluviosidade", cor: "#10b981" },
    { nome: "Velocidade_vento", cor: "#6366f1" }
  ];

  return (
    <div className="pagina-container">
      <div className="seletor-range">
        <label>Intervalo:</label>
        <select value={range} onChange={e => setRange(e.target.value)}>
          <option value="-1h">Última 1 hora</option>
          <option value="-6h">Últimas 6 horas</option>
          <option value="-12h">Últimas 12 horas</option>
          <option value="-24h">Últimas 24 horas</option>
          <option value="-3d">Últimos 3 dias</option>
          <option value="-5d">Últimos 5 dias</option>
        </select>
      </div>

      {loading ? (
        <p className="loading-text">Carregando dados...</p>
      ) : (
        <div className="graficos-grid">
          {campos.map(campo => renderChart(campo.nome, campo.cor))}
        </div>
      )}
    </div>
  );
};

export default DadosMeteo;
