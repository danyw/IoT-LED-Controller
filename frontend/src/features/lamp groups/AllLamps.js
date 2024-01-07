import { useState } from 'react';
import Header from '../../components/Header';
import lampGroups from '../../data/mockLampGroups';
import {
  Box,
  Select,
  MenuItem,
  Button,
  FormControl,
  InputLabel,
  Typography,
  useTheme,
} from '@mui/material';
import { DataGrid } from '@mui/x-data-grid';
import { tokens } from '../../config/theme';

const AllLamps = () => {
  const [selectedGroup, setSelectedGroup] = useState(
    Object.keys(lampGroups)[0]
  );
  const theme = useTheme();
  const colors = tokens(theme.palette.mode);

  const columns = [
    { field: 'id', headerName: 'ID' },
    {
      field: 'spectrum',
      headerName: 'Spectrum',
      flex: 1,
      cellClassName: 'name-column--cell',
    },
    {
      field: 'isOn',
      headerName: 'Status',
      flex: 1,
      renderCell: (params) => (
        <Typography
          color={
            params.row.isOn
              ? `${colors.yellowAccent[600]}`
              : `${colors.redAccent[500]}`
          }
          fontSize="1rem"
        >
          {params.row.isOn ? 'On' : 'Off'}
        </Typography>
      ),
    },
  ];

  const handleGroupChange = (event) => {
    setSelectedGroup(event.target.value);
  };

  const handleAddGroup = () => {
    // TODO
  };

  const handleDeleteGroup = () => {
    // TODO
  };

  const handleAddLamp = () => {
    // TODO
  };

  const handleShowDetails = () => {
    // TODO
  };

  const handleDeleteSelectedLamps = () => {
    // TODO
  };

  return (
    <Box m={{ xs: '10px', md: '20px' }} maxWidth="100%">
      <Header
        title="Complete Lamp Collections"
        subtitle="Each group contains a set of LED lamps"
      />
      <FormControl fullWidth margin="normal">
        <InputLabel id="lamp-group-select-label">Lamp group</InputLabel>
        <Select
          labelId="lamp-group-select-label"
          id="lamp-group-select"
          value={selectedGroup}
          label="Lamp Group"
          onChange={handleGroupChange}
        >
          {Object.keys(lampGroups).map((group) => (
            <MenuItem key={group} value={group}>
              {group}
            </MenuItem>
          ))}
        </Select>
      </FormControl>

      <Button
        variant="contained"
        color="primary"
        onClick={handleAddGroup}
        sx={{ margin: '0.3rem' }}
      >
        Add a New Group
      </Button>
      <Button
        variant="contained"
        color="error"
        onClick={handleDeleteGroup}
        sx={{ margin: '0.3rem' }}
      >
        Delete a Group
      </Button>

      <Typography variant="h3" sx={{ mt: 2, mb: 2 }}>
        Lamps in {lampGroups[selectedGroup].groupName} -{' '}
        {lampGroups[selectedGroup].room}:
      </Typography>

      {/* {lampGroups[selectedGroup].lamps.map((lamp) => (
        <Typography key={lamp.id}>
          {lamp.id} - Wattage: {lamp.wattage}W - Spectrum: {lamp.spectrum} -
          Status: {lamp.isOn}
        </Typography>
      ))} */}

      <Box
        m="5px 0 0 0"
        max-height="75vh"
        sx={{
          '& .MuiDataGrid-root': {
            border: 'none',
            fontSize: '1rem',
          },
          '& .MuiDataGrid-cell': {
            borderBottom: 'none',
            fontSize: '1rem',
          },
          '& .name-column--cell': {
            color: colors.yellowAccent[300],
            fontSize: '1rem',
          },
          '& .MuiDataGrid-columnHeaders': {
            backgroundColor: colors.blueAccent[700],
          },
          '& .MuiDataGrid-virtualScroller': {
            backgroundColor: colors.primary[400],
          },
          '& .MuiDataGrid-footerContainer': {
            borderTop: 'none',
            backgroundColor: colors.blueAccent[700],
          },
          '& .MuiCheckbox-root': {
            color: `${colors.yellowAccent[200]} !important`,
          },
        }}
      >
        <DataGrid
          checkboxSelection
          rows={lampGroups[selectedGroup].lamps}
          columns={columns}
        />
        <Button
          variant="contained"
          color="primary"
          onClick={handleShowDetails}
          sx={{ margin: '0.3rem' }}
        >
          Details
        </Button>
        <Button
          variant="contained"
          color="primary"
          onClick={handleAddLamp}
          sx={{ margin: '0.3rem' }}
        >
          Add a New Lamp
        </Button>
        <Button
          variant="contained"
          color="error"
          onClick={handleDeleteSelectedLamps}
          sx={{ margin: '0.3rem' }}
        >
          Delete Selected Lamps
        </Button>
      </Box>
    </Box>
  );
};

export default AllLamps;
