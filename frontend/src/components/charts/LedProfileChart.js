import React from 'react';
import { ResponsiveLine } from '@nivo/line';
import { linearGradientDef } from '@nivo/core';
import { prepareLedData } from './prepareLedData';
import ledProfiles from '../../data/ledProfiles';

const LedProfileChart = ({ profileName }) => {
  if (!profileName || !ledProfiles[profileName]) {
    return <div>No LED profile selected or profile does not exist.</div>;
  }
  const ledProfile = ledProfiles[profileName];
  const data = prepareLedData([ledProfile]);

 

  // make sure parent container have a defined height when using
  // responsive component, otherwise height will be 0 and
  // no chart will be rendered.
  return (
    <ResponsiveLine
      data={data}
      margin={{ top: 50, right: 110, bottom: 50, left: 60 }}
      xScale={{
        type: 'linear',
        min: 380,
        max: 780,
      }}
      yScale={{
        type: 'linear',
        min: 0,
        max: 100,
        stacked: true,
        reverse: false,
      }}
      yFormat=" >-.2f"
      curve="monotoneX"
      axisTop={null}
      axisRight={null}
      axisBottom={{
        tickSize: 5,
        tickPadding: 5,
        tickRotation: 0,
        legend: 'transportation',
        legendOffset: 36,
        legendPosition: 'middle',
      }}
      axisLeft={{
        tickSize: 5,
        tickPadding: 5,
        tickRotation: 0,
        legend: 'count',
        legendOffset: -40,
        legendPosition: 'middle',
      }}
      colors={{ scheme: 'spectral' }}
      enablePoints={false}
      pointSize={10}
      pointColor={{ theme: 'background' }}
      pointBorderWidth={2}
      pointBorderColor={{ from: 'serieColor' }}
      pointLabelYOffset={-12}
      enableArea={true}
      areaOpacity={0.5}
      useMesh={true}
      legends={[
        {
          anchor: 'bottom-right',
          direction: 'column',
          justify: false,
          translateX: 100,
          translateY: 0,
          itemsSpacing: 0,
          itemDirection: 'left-to-right',
          itemWidth: 80,
          itemHeight: 20,
          itemOpacity: 0.75,
          symbolSize: 12,
          symbolShape: 'circle',
          symbolBorderColor: 'rgba(0, 0, 0, .5)',
          effects: [
            {
              on: 'hover',
              style: {
                itemBackground: 'rgba(0, 0, 0, .03)',
                itemOpacity: 1,
              },
            },
          ],
        },
      ]}
      fill={[{ match: '*', id: 'gradientX' }]}
      defs={[
        // Define the gradient
        linearGradientDef(
          'gradientX',
          [
            { offset: 0, color: '#610061' },     // Violet start at 380nm
            { offset: ((450 - 380) / (780 - 380)) * 100, color: '#0046ff' }, // Violet end at 450nm
            { offset: ((495 - 380) / (780 - 380)) * 100, color: '#00ffcbf' }, // Blue end at 495nm
            { offset: ((570 - 380) / (780 - 380)) * 100, color: '#e1ff00' }, // Green end at 570nm
            { offset: ((590 - 380) / (780 - 380)) * 100, color: '#ffdf00' }, // Yellow end at 590nm
            { offset: ((620 - 380) / (780 - 380)) * 100, color: '#ff7700' }, // Orange end at 620nm
            { offset: 100, color: '#610000' },                                // Red end at 780nm
          ],
          { x1: 0, x2: 1, y1: 0, y2: 0 }
        ), // Horizontal gradient
      ]}
    />
  );
};

export default LedProfileChart;
