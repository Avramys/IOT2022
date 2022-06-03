import React, { useState,  } from 'react';
import HookMqtt from './components/Hook/'
// Hook or Class
// import ClassMqtt from './components/Class/'
import './App.css';
import ViewPrincipal from './components/View/index'

function App() {

  const [layoutPage, setLayoutPage] = useState(true);
  const [payload, setPayloadState] = useState([]);
  const [messageToPublish, setMessageToPublish] = useState(null)

  const setStatePerson = () => {
   setLayoutPage(!layoutPage);
  };

  const setStateOfPayload = (value) => {
    setPayloadState(value);
  }

  const setStateOfMessageToPublish = (value) => {
    setMessageToPublish(value);
    console.log(value)
  }
  

  return (
    <div className={"App"} >
      <div className={layoutPage ? 'displayN' : ''}>
        <HookMqtt setPayload={setStateOfPayload} messageToPublish={messageToPublish}></HookMqtt>
      </div>
      <ViewPrincipal setStatePersonState={setStatePerson} payload={payload} setStateOfMessageToPublish={setStateOfMessageToPublish}></ViewPrincipal>
    </div>
  );
}


export default App;
