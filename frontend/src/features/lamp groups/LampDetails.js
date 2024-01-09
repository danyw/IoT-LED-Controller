import Header from '../../components/Header';
import { useLocation } from 'react-router-dom';
import {
  Box,
  Typography,
  useTheme,
  Slider,
  Input,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  Table,
  TableBody,
  TableCell,
  Button,
} from '@mui/material';
import { tokens } from '../../config/theme';
import { useState, useEffect } from 'react';
import LightSchedule from './LightSchedule';
import LightScheduleChart from '../../components/charts/LightScheduleChart';
import mockScheduleProfiles from '../../data/mockScheduleProfiles';
import ScheduleSelector from './ScheduleSelector';
import { transformProfileToSchedule } from './transformProfileToSchedule';
import { sendLedControlRequest } from '../../api/ledControlApi';

// TODO
const fetchLampDetails = async (lampId) => {
  return {
    id: lampId,
    name: 'Lamp01',
    status: 'Active',
    watts: 60,
    temperature: 35,
    efficiency: 90,
    current: 0,
    voltage: 0
  };
};

const LampDetails = () => {
  const location = useLocation();
  const { selectedLampId } = location.state || { selectedLampId: null };
  const theme = useTheme();
  const colors = tokens(theme.palette.mode);
  const [colorValue, setColorValue] = useState({
    'warm white': 0,
    'cool white': 0,
    green: 0,
    blue: 0,
    red: 0,
  });
  const [lampData, setLampData] = useState(null);
  const [tempColorValue, setTempColorValue] = useState({});

  const ledColors = ['warm white', 'cool white', 'green', 'blue', 'red'];

  const [selectedProfileName, setSelectedProfileName] = useState(
    Object.keys(mockScheduleProfiles)[0]
  );

  const initialProfileData = mockScheduleProfiles[selectedProfileName];
  const initialSchedule = transformProfileToSchedule(initialProfileData);
  const [schedule, setSchedule] = useState(initialSchedule);
  useEffect(() => {
    if (selectedLampId) {
      fetchLampDetails(selectedLampId).then((data) => {
        setLampData(data);
      });
    }
  }, [selectedLampId]);

  const colorMap = {
    'warm white': '#FFD580', // Example color code
    'cool white': '#E0E0E0',
    green: '#8FBC8F',
    blue: '#ADD8E6',
    red: '#FF6347',
  };

  const handleSliderChange = (color) => (event, newValue) => {
    setColorValue((prev) => ({
      ...prev,
      [color]: newValue,
    }));

    setTempColorValue((prev) => ({
      ...prev,
      [color]: newValue,
    }));
  };

  const handleSliderChangeCommitted = (color) => (event, newValue) => {
    sendLedControlRequest(color, newValue);
  };

  const handleInputChange = (color) => (event) => {
    setTempColorValue({ ...tempColorValue, [color]: event.target.value });
  };

  const handleBlur = (color) => () => {
    setColorValue((prev) => ({
      ...prev,
      [color]: tempColorValue[color] === '' ? 0 : Number(tempColorValue[color]),
    }));
    sendLedControlRequest(color, tempColorValue[color]);
  };

  const handleKeyPress = (color) => (event) => {
    if (event.key === 'Enter') {
      setColorValue((prev) => ({
        ...prev,
        [color]:
          tempColorValue[color] === '' ? 0 : Number(tempColorValue[color]),
      }));
      sendLedControlRequest(color, tempColorValue[color]);
    }
  };

  const handleScheduleChange = (hour, colorIndex, value) => {
    const newSchedule = [...schedule];
    newSchedule[hour][colorIndex] = Math.max(0, Math.min(1000, Number(value)));
    setSchedule(newSchedule);
  };

  const handleProfileChange = (profileName) => {
    const newProfileData = mockScheduleProfiles[profileName];
    const newSchedule = transformProfileToSchedule(newProfileData);
    setSchedule(newSchedule);
    setSelectedProfileName(profileName);
  };

  const chartData = ledColors.map((color, index) => ({
    id: color,
    data: schedule.map((hourData, hour) => ({
      x: hour,
      y: hourData[index],
    })),
  }));

  const handleApplyProfile = () => {

  }

  // console.log(JSON.stringify(chartData, null, 2));

  return (
    <Box m={{ xs: '10px', md: '20px' }} maxWidth="100%">
      <Header
        title="Lamp Details"
        subtitle="Live control and data collection"
      />
      <Box padding={3} maxWidth="100%">
        <Box
          sx={{
            display: 'flex',
            flexDirection: { sm: 'column', md: 'row', xs: 'column' },
            justifyContent: 'flex-start',
            alignItems: { sm: 'left', xs: 'left', md: 'center' },
            maxWidth: '1200px',
          }}
        >
          <Box flexGrow={1}>
            {['warm white', 'cool white', 'green', 'blue', 'red'].map(
              (color) => (
                <Box
                  key={color}
                  marginTop={2}
                  sx={{
                    display: 'flex',
                    flexDirection: 'row',
                  }}
                >
                  <Box maxWidth={400} marginRight={2} flexGrow={1}>
                    <Typography variant="h6" id="input-slider" gutterBottom>
                      {color.toUpperCase()}
                    </Typography>

                    <Slider
                      value={colorValue[color]}
                      onChange={handleSliderChange(color)}
                      onChangeCommitted={handleSliderChangeCommitted(color)}
                      aria-labelledby="continuous-slider"
                      style={{ maxWidth: '400px', color: colorMap[color] }}
                      step={1}
                      min={0}
                      max={1000}
                    />
                  </Box>

                  <Input
                    value={tempColorValue[color] || colorValue[color]}
                    size="small"
                    onChange={handleInputChange(color)}
                    onBlur={handleBlur(color)}
                    onKeyPress={handleKeyPress(color)}
                    inputProps={{
                      step: 1,
                      min: 0,
                      max: 1000,
                      type: 'number',
                      'aria-labelledby': 'input-slider',
                    }}
                  />
                </Box>
              )
            )}
          </Box>
          {lampData && (
            <TableContainer component={Paper} sx={{ maxWidth: 400 }}>
              <Table
                sx={{ minWidth: 200, maxWidth: 400 }}
                aria-label="simple table"
              >
                <TableHead>
                  <TableRow>
                    <TableCell>Details:</TableCell>
                    <TableCell align="right"></TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {Object.entries(lampData).map(([key, value]) => (
                    <TableRow key={key}>
                      <TableCell component="th" scope="row">
                        {key}
                      </TableCell>
                      <TableCell align="right">{value}</TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
            </TableContainer>
          )}
        </Box>
      </Box>
      <Typography variant="h3" paddingTop={5}>
        Light Schedule
      </Typography>
      <ScheduleSelector
        selectedProfile={selectedProfileName}
        onProfileChange={handleProfileChange}
        ledColors={ledColors}
      />
      <Box
        height="100%"
        paddingTop={5}
        sx={{
          display: 'flex',
          flexDirection: { sm: 'column', md: 'row', xs: 'column' },
          // flexDirection: 'row',
          maxWidth: '100%',
          flexWrap: 'nowrap',
        }}
      >
        <Box minWidth={500}>
        <LightSchedule
          ledColors={ledColors}
          schedule={schedule}
          onScheduleChange={handleScheduleChange}
        />
        </Box>
        <Box height="50vh"   flexGrow={1} sx={{
          maxWidth: { sm: '90vw', md: '50vw'},
        }}  >
          <LightScheduleChart data={chartData} />
          <Button variant="contained" color="primary" onClick={handleApplyProfile} sx={{ marginLeft: '2rem' }} >
        Apply the profile
      </Button>
        </Box>
      </Box>
    </Box>
  );
};

export default LampDetails;
