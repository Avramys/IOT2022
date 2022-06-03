import React, { useEffect, useState } from "react";
import {  Label,Text, Toggle, PrimaryButton, TextField,  IDatePickerStrings, Dropdown, getPropsWithDefaults  } from 'office-ui-fabric-react';
import styles from './styles.css';
import ReactSpeedometer from "react-d3-speedometer";
import InputNumber from 'react-input-number';

function ViewPrincipal({setStatePersonState, payload, setStateOfMessageToPublish}) {
    const [record, setRecord] = useState({topic: "testtopic/react", qos: 0,})
    const [num, setNum] = useState(20);
    const [valueSpeed, setValueSpeed] = useState(0);
    const [controleAutomatico, setControleAutomatico] = useState(false);
    const [ligaLedMcu, setLigaLedMcu] = useState(0);
    const [arCondicionado, setArCondicionado] = useState(false);

    

    const onRecordChange = (value) => {
        const changedRecord = Object.assign(record, value);
        setRecord(changedRecord)
      };

    const handleSubscribe = () => {
       console.log('teste')
    };

    useEffect(() => {
            
        if (payload.topic == 'testtopic/info/temp') {                                                                                                                                                                                               const resolveTemp = fahrenheit => fahrenheit;                                                                                                                                                                                    //const fahrenheitToCelsius = fahrenheit => (fahrenheit - 32) * 5/9;
            setValueSpeed(resolveTemp(parseInt(payload.message)).toFixed(0));               
        }
            

        if(valueSpeed < num) {
            if(arCondicionado) {
                alteraArCondicionado()
            }
        }

        if(valueSpeed > num && controleAutomatico && !arCondicionado) {
            alteraArCondicionado();
        }

    })

    const alteraNodeMcu = () => {
        setLigaLedMcu(!ligaLedMcu);
        setStateOfMessageToPublish(
            { topic: 'testtopic/demo/led16', qos: 0, payload: `${!ligaLedMcu}` } );
    }

    const alteraAutoMode = () => {
        setControleAutomatico(!controleAutomatico)
        setStateOfMessageToPublish(
            { topic: 'testtopic/ac/auto', qos: 0, payload: `${!controleAutomatico}` } );
    }

    const alteraArCondicionado = () => {
        setArCondicionado(!arCondicionado);
    }


    const setAC = (value) => {
        setNum(value);
    }



    return (
        <div className={ styles.sjkFormularioUploadDocs } style={{width: '50%'}}>
            <div className={ styles.container } style={{display: 'flex', justifyContent: 'space-around'}}>
                <div className={ styles.row }>
                    <Label className={styles.label} style={{color: 'green', fontSize: 30}}>Demo</Label>
                    
                    <div className={ styles.column }>
                        <Label className={styles.label} style={{color: 'white', fontSize: 20}}>Ligar TV</Label>
                        <PrimaryButton >Ligar/Desligar TV</PrimaryButton>
                    </div>
                    <div className={ styles.column }>
                        <Label className={styles.label} style={{color: 'white', fontSize: 20}}>Ligar Led Nodemcu</Label>
                        <Toggle
                            onChange={alteraNodeMcu}
                            value={ligaLedMcu}
                        ></Toggle>
                    </div>
                    
                    <div className={ styles.column }>
                        <Label className={styles.label} style={{color: 'white', fontSize: 20}}>Sensores</Label>
                        <ReactSpeedometer 
                            minValue={-30}
                            maxValue={60}
                            value={valueSpeed}
                            startColor="#79f7f7"
                            segments={100}
                            endColor="red"
                            maxSegmentLabels={10}
                            textColor={'white'}

                        />
                    </div>

                    
                </div>

                <div className={ styles.row }>
                    <Label className={styles.label} style={{color: 'green', fontSize: 30}}>Comandos</Label>
                    
                    <div className={ styles.column }>
                        <Label className={styles.label} style={{color: 'white', fontSize: 20}}>Ar condicionado</Label>
                        <Toggle
                           onChange={alteraArCondicionado}
                           checked={arCondicionado}
                        ></Toggle>
                    </div>

                    <div className={ styles.column }>
                        <Label className={styles.label} style={{color: 'white', fontSize: 20}}>Controle Automatico</Label>
                        <Toggle
                            onChange={alteraAutoMode}
                        ></Toggle>
                    </div>

                    <div className={ styles.column }>
                        <Label className={styles.label} style={{color: 'white', fontSize: 20}}>Temperatura AC</Label>
                        <input
                            type="number"
                            min={10}
                            max={50}
                            step={1}
                            height={10}
                            value={num}
                            style={{background: '#282c34', color: 'white'}}
                            onChange={e => {setAC(e.target.value)}}
                        />
                    </div>

                   
                </div>
            </div>
            <PrimaryButton onClick={setStatePersonState}>Botão</PrimaryButton>
        
        </div>
    );
}



export default ViewPrincipal;
