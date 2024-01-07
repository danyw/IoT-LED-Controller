import Header from '../../components/Header';
import LedProfileChart from '../../components/charts/LedProfileChart';
import {
  Box,
  Select,
  MenuItem,
  Button,
  FormControl,
  InputLabel,
} from '@mui/material';
import ledProfiles from '../../data/ledProfiles';
import React, { useState } from 'react';

const LedProfiles = () => {
  const [selectedProfile, setSelectedProfile] = useState('Cool White');

  const handleProfileChange = (event) => {
    setSelectedProfile(event.target.value);
  };
  const handleAddProfile = () => {
    // TODO
  };

  const handleDeleteProfile = () => {
    // TODO
  };

  const currentProfile = ledProfiles[selectedProfile];

  return (
    <Box m={{ xs: '10px', md: '20px' }} maxWidth="100%">
      <Header title="LED Profiles" subtitle="List of LED profiles." />
      <FormControl fullWidth margin="normal">
        <InputLabel id="led-profile-select-label">LED Profile</InputLabel>
        <Select
          labelId="led-profile-select-label"
          id="led-profile-select"
          value={selectedProfile}
          label="LED Profile"
          onChange={handleProfileChange}
        >
          {Object.keys(ledProfiles).map((profile) => (
            <MenuItem key={profile} value={profile}>
              {profile}
            </MenuItem>
          ))}
        </Select>
      </FormControl>
      <Button variant="contained" color="primary" onClick={handleAddProfile} sx={{ margin: '0.3rem' }} >
        Add a New Profile
      </Button>
      <Button variant="contained" color="error" onClick={handleDeleteProfile} sx={{ margin: '0.3rem' }} >
        Delete a Profile
      </Button>
      <Box height="50vh">
        <LedProfileChart profileName={selectedProfile} />
      </Box>
      <Box>
        <p><strong>Current Profile:</strong></p>
        <p>Model: {currentProfile.model}</p>
        <p>Wattage: {currentProfile.wattage}W</p>
      </Box>
    </Box>
  );
};

export default LedProfiles;
