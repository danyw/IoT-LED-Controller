import { ResponsiveLine } from '@nivo/line';
import { useTheme } from '@mui/material';
import { tokens } from '../../config/theme';

const LightScheduleChart = ({ data }) => {
  const theme = useTheme();
  const colors = tokens(theme.palette.mode);
  const lineColors = [colors.yellowAccent[500], colors.white[400], '#32CD32', colors.blueAccent[500], '#ff0000']; 
  return (
    <ResponsiveLine
      data={data}
      margin={{ top: 30, right: 100, bottom: 50, left: 60 }}
      xScale={{ type: 'linear', min: 0, max: 24 }}
      yScale={{
        type: 'linear',
        min: 0,
        max: 1000,
        stacked: false,
        reverse: false,
      }}
      yFormat=" >-.2f"
      axisTop={null}
      axisRight={null}
      axisBottom={{
        tickSize: 5,
        tickPadding: 5,
        tickRotation: 0,
        legend: 'Time',
        legendOffset: 36,
        legendPosition: 'middle',
      }}
      axisLeft={{
        tickSize: 5,
        tickPadding: 5,
        tickRotation: 0,
        legend: 'Intensity',
        legendOffset: -40,
        legendPosition: 'middle',
      }}
      theme={{
        axis: {
          domain: {
            line: {
              stroke: colors.white[100],
            },
          },
          legend: {
            text: {
              fill: colors.white[100],
            },
          },

          ticks: {
            line: {
              stroke: colors.white[100],
              strokeWidth: 1,
            },
            text: {
              fill: colors.white[100],
            },
          },
        },
        legends: {
          text: {
            fill: colors.white[100],
          },
        },
        tooltip: {
          container: {
            color: colors.yellowAccent[700],
          },
        },
      }}
      // colors={{ scheme: 'spectral' }}
      colors={lineColors}
      enablePoints={false}
      pointSize={10}
      pointColor={{ theme: 'background' }}
      pointBorderWidth={2}
      pointBorderColor={{ from: 'serieColor' }}
      pointLabelYOffset={-12}
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
    />
  );
};

export default LightScheduleChart;
