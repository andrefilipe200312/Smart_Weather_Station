import React from 'react';
import './ClimaBoxes.css';

const ClimaBoxes = ({ dadosClima, loading, erro }) => {
  // Função para formatar timestamp
  const formatarData = (timestamp) => {
    if (!timestamp) return 'N/A';
    const date = new Date(timestamp);
    return date.toLocaleString('pt-PT');
  };

  // Função para obter o timestamp mais recente dos dados
  const obterTimestampMaisRecente = () => {
    if (!dadosClima) return null;
    let timestampMaisRecente = null;
    
    Object.values(dadosClima).forEach(campo => {
      if (campo && campo.time) {
        const time = new Date(campo.time);
        if (!timestampMaisRecente || time > timestampMaisRecente) {
          timestampMaisRecente = time;
        }
      }
    });
    
    return timestampMaisRecente;
  };

  // Função para renderizar valor com tratamento de dados nulos
  const renderValue = (campo, unidade = '') => {
    if (!campo || campo.value === null || campo.value === undefined) {
      return <span className="no-data">N/A</span>;
    }
    return `${campo.value}${unidade}`;
  };

  return (
    <div className="clima-boxes-container">   
      {loading && (
        <div className="clima-loading">
          Carregando dados climáticos
        </div>
      )}
      
      {erro && (
        <div className="clima-error">
          {erro}
        </div>
      )}

      {dadosClima && (
        <>
          <p className="clima-timestamp">
            Última atualização: {formatarData(obterTimestampMaisRecente())}
          </p>
          
          <div className="clima-boxes-grid">
            <div className="clima-box">
              <h3 className="clima-box-title">
                <span>🌡️</span>
                Temperatura
              </h3>
              <div className="clima-box-value">
                {renderValue(dadosClima.Temperatura, '°C')}
              </div>
            </div>

            <div className="clima-box">
              <h3 className="clima-box-title">
                <span>💧</span>
                Humidade
              </h3>
              <div className="clima-box-value">
                {renderValue(dadosClima.Humidade, '%')}
              </div>
            </div>

            <div className="clima-box">
              <h3 className="clima-box-title">
                <span>🧭</span>
                Direção do Vento
              </h3>
              <div className="clima-box-value">
                {renderValue(dadosClima.Direcao_vento, '°')}
              </div>
            </div>

            <div className="clima-box">
              <h3 className="clima-box-title">
                <span>💨</span>
                Velocidade do Vento
              </h3>
              <div className="clima-box-value">
                {renderValue(dadosClima.Velocidade_vento, ' km/h')}
              </div>
            </div>

            <div className="clima-box">
              <h3 className="clima-box-title">
                <span>🌧️</span>
                Pluviosidade
              </h3>
              <div className="clima-box-value">
                {renderValue(dadosClima.Pluviosidade, ' mm')}
              </div>
            </div>
          </div>
        </>
      )}
    </div>
  );
};

export default ClimaBoxes;