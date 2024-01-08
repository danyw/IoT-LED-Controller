import React, { useState } from 'react';
import {
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  Input,
} from '@mui/material';

const LightSchedule = ({ ledColors, schedule, onScheduleChange }) => {
  // Initialize schedule state based on the provided LED colors
//   const [schedule, setSchedule] = useState(
//     Array.from({ length: 24 }, () => Array(ledColors.length).fill(0))
//   );

  const handleInputChange = (hour, colorIndex, value) => {
    onScheduleChange(hour, colorIndex, value);
  };

  return (
    <TableContainer component={Paper}  sx={{ maxWidth: 650 }}>
      <Table  size="small" aria-label="a dense table">
        <TableHead>
          <TableRow>
            <TableCell>Time</TableCell>
            {ledColors.map((color) => (
              <TableCell key={color}>{color}</TableCell>
            ))}
          </TableRow>
        </TableHead>
        <TableBody>
          {schedule.map((row, hour) => (
            <TableRow key={hour}>
              <TableCell>{`${hour}:00`}</TableCell>
              {row.map((value, colorIndex) => (
                <TableCell key={colorIndex}>
                  <Input
                    type="number"
                    value={value}
                    onChange={(e) =>
                      handleInputChange(hour, colorIndex, e.target.value)
                    }
                    inputProps={{ min: 0, max: 1000 }}
                  />
                </TableCell>
              ))}
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </TableContainer>
  );
};

export default LightSchedule;
