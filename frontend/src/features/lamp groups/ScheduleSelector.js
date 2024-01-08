import React from 'react';
import { FormControl, InputLabel, Select, MenuItem } from '@mui/material';
import mockScheduleProfiles from '../../data/mockScheduleProfiles';
import { transformProfileToSchedule } from './transformProfileToSchedule'; 

const ScheduleSelector = ({ selectedProfile, onProfileChange, ledColors }) => {
    const handleProfileChange = (event) => {
      // Call the onProfileChange with the new profile name
      onProfileChange(event.target.value);
    };
  
    return (
      <FormControl fullWidth margin="normal">
        <InputLabel id="schedule-profile-select-label">Schedule Profile</InputLabel>
        <Select
          labelId="schedule-profile-select-label"
          id="schedule-profile-select"
          value={selectedProfile}
          label="Schedule Profile"
          onChange={handleProfileChange}
        >
          {Object.keys(mockScheduleProfiles).map((profile) => (
            <MenuItem key={profile} value={profile}>
              {profile}
            </MenuItem>
          ))}
        </Select>
      </FormControl>
    );
  };
  

export default ScheduleSelector;
