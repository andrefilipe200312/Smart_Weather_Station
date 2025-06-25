import React from 'react';
import './Sobre.css';
import Navbar from '../Components/Navbar'; 
const Sobre = () => {
  return (
    <div>
      <Navbar />
      <main className="sobre-content">
        <div className="sobre-caixa">
          <p>
            Somos o Rafael e o André, dois estudantes da licenciatura em Engenharia Informática e de Computadores, na escola Náutica Infante D.Henrique e este é o nosso projeto final de curso: uma estação meteorológica com tudo o que é preciso para acompanhar o tempo em tempo real, de forma simples e prática.
          </p>
          <p>
           A ideia surgiu da vontade de pôr em prática aquilo que aprendemos ao longo dos últimos anos — desde sensores e microcontroladores até ao desenvolvimento de interfaces web. Juntámos tudo isso neste projeto, que combina hardware, software e (muita) persistência.
          </p>
          <p>
            Este site é a cara visível do nosso trabalho. Esperamos que gostes de explorar, e quem sabe, que te inspires a criar algo também!
          </p>
          <p> 
            Se tiveres dúvidas, sugestões ou apenas quiseres trocar ideias, estás à vontade para nos contactar.
          </p>
          <p>
            André Filipe Pinheiro Ferreira, 13741@enautica.pt  
          </p>
          <p>
            Rafael Alexandre Sampaio Baraçal, 13751@enautica.pt  
          </p>
        </div>
      </main>
    </div>
  );
};

export default Sobre;
