import React from 'react';

const DashLamp = () => {
    const sliders = [
        { name: 'Brightness', value: 50 },
        { name: 'Color Temperature', value: 3000 },
        { name: 'Saturation', value: 80 },
    ];

    return (
        <div>
            <h1>Lamp Dashboard</h1>
            {sliders.map((slider, index) => (
                <div key={index}>
                    <label>{slider.name}</label>
                    <input type="range" min="0" max="100" value={slider.value} />
                </div>
            ))}
        </div>
    );
};

export default DashLamp;
