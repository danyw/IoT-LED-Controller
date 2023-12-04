import React, { useState } from 'react';
import { ResponsiveLine } from '@nivo/line';
import { Box, Typography, Slider } from '@mui/material';

const LampDetails = () => {
    const [expandList, setExpandList] = useState(false);
    const [colorValue, setColorValue] = useState({
        uv: 50,
        blue: 50,
        red: 50,
    });

    const handleSliderChange = (color) => (event, newValue) => {
        setColorValue((prev) => ({
            ...prev,
            [color]: newValue,
        }));
    };

    const lineData = [
        {
            id: 'intensity',
            color: 'hsl(194, 70%, 50%)',
            data: [
                { x: '430nm', y: colorValue.uv },
                { x: '550nm', y: colorValue.blue },
                { x: '660nm', y: colorValue.red },
            ],
        },
    ];

    return (
        <Box padding={3}>
            <Box display="flex">
                <Box flexGrow={1} marginRight={3}>
                    {['uv', 'blue', 'red'].map((color) => (
                        <Box key={color} marginTop={2}>
                            <Typography variant="h6">{color.toUpperCase()}</Typography>
                            <Slider
                                value={colorValue[color]}
                                onChange={handleSliderChange(color)}
                                aria-labelledby="continuous-slider"
                                style={{ maxWidth: '100px' }}
                            />
                        </Box>
                    ))}
                </Box>
                <Box width={200}>
                    <Typography variant="h6">Lamp Details:</Typography>
                    <Typography>Lamp Name: XYZ</Typography>
                    <Typography>Status: Active</Typography>
                    <Typography>Health: Good</Typography>
                    <Typography>Watts: 60W</Typography>
                    <Typography>Temperature: 35°C</Typography>
                    <Typography>Efficiency: 90%</Typography>
                    <Typography>Humidity: 50%</Typography>
                    <Typography>Air Temp: 25°C</Typography>
                </Box>
            </Box>
            <Box marginTop={3} height={400}>
                <ResponsiveLine
                    data={lineData}
                    margin={{ top: 50, right: 50, bottom: 50, left: 60 }}
                    xScale={{ type: "point" }}
                    yScale={{
                        type: "linear",
                        min: "auto",
                        max: "auto",
                        stacked: true,
                        reverse: false,
                    }}
                    curve="monotoneX"
                    axisBottom={{
                        orient: "bottom",
                        tickSize: 5,
                        tickPadding: 5,
                        tickRotation: 0,
                        legend: "spectrum (nm)",
                        legendOffset: 36,
                        legendPosition: "middle",
                        tickValues: ["430nm", "550nm", "660nm"],
                        tickStroke: "white",
                        legendColor: "white",
                        tickColor: "white",
                        legendOpacity: 1,
                    }}
                    axisLeft={{
                        orient: "left",
                        tickSize: 5,
                        tickPadding: 5,
                        tickRotation: 0,
                        legend: "intensity",
                        legendOffset: -40,
                        legendPosition: "middle",
                        tickStroke: "white",
                        tickColor: "white",
                    }}
                    colors={{ scheme: "nivo" }}
                    pointSize={10}
                    pointColor={{ theme: "background" }}
                    pointBorderWidth={2}
                    pointBorderColor={{ from: "serieColor" }}
                    pointLabel="y"
                    pointLabelYOffset={-12}
                    useMesh={true}
                    theme={{
                        axis: {
                            ticks: {
                                text: {
                                    fill: "white",
                                },
                            },
                            legend: {
                                text: {
                                    fill: "white",
                                },
                            },
                        },
                        grid: {
                            line: {
                                stroke: "#888",
                                strokeWidth: 1,
                            },
                        },
                    }}
                />
            </Box>
        </Box>
    );
};

export default LampDetails;
