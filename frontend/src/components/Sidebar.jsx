import { useState } from 'react';
import { Link } from 'react-router-dom';
import { ProSidebar, Menu, MenuItem } from 'react-pro-sidebar';
import 'react-pro-sidebar/dist/css/styles.css';
import { tokens } from '../config/theme';
import {
  Box,
  IconButton,
  Typography,
  useTheme,
  Select,
  FormControl,
  MenuItem as MuiMenuItem,
} from '@mui/material';
import plant from '../assets/plant.png';

import HomeOutlinedIcon from '@mui/icons-material/HomeOutlined';
import PieChartOutlineOutlinedIcon from '@mui/icons-material/PieChartOutlineOutlined';
import MenuOutlinedIcon from '@mui/icons-material/MenuOutlined';
import WorkspacesOutlinedIcon from '@mui/icons-material/WorkspacesOutlined';
import LightModeOutlinedIcon from '@mui/icons-material/LightModeOutlined';
import WaterDropOutlinedIcon from '@mui/icons-material/WaterDropOutlined';
import DataUsageOutlinedIcon from '@mui/icons-material/DataUsageOutlined';
import EnergySavingsLeafOutlinedIcon from '@mui/icons-material/EnergySavingsLeafOutlined';
import CreateNewFolderOutlinedIcon from '@mui/icons-material/CreateNewFolderOutlined';
import FolderSpecialOutlinedIcon from '@mui/icons-material/FolderSpecialOutlined';
import TipsAndUpdatesOutlinedIcon from '@mui/icons-material/TipsAndUpdatesOutlined';

const Item = ({ title, to, icon, selected, setSelected }) => {
  const theme = useTheme();
  const colors = tokens(theme.palette.mode);
  return (
    <MenuItem
      active={selected === title}
      style={{
        color: colors.white[100],
      }}
      onClick={() => setSelected(title)}
      icon={icon}
    >
      <Typography>{title}</Typography>
      <Link to={to} />
    </MenuItem>
  );
};

const Sidebar = () => {
  const theme = useTheme();
  const colors = tokens(theme.palette.mode);
  const [isCollapsed, setIsCollapsed] = useState(false);
  const [selected, setSelected] = useState('Dashboard');
  
  return (
    <Box
      sx={{
        '& .pro-sidebar-inner': {
          background: `${colors.primary[700]} !important`,
        },
        '& .pro-icon-wrapper': {
          backgroundColor: 'transparent !important',
        },
        '& .pro-inner-item': {
          padding: '5px 35px 5px 20px !important',
        },
        '& .pro-inner-item:hover': {
          color: `${colors.primary[400]} !important`,
        },
        '& .pro-menu-item.active': {
          color: `${colors.blueAccent[500]} !important`,
        },
      }}
    >
      <ProSidebar collapsed={isCollapsed} >
        <Menu iconShape="square">
          {/* LOGO AND MENU ICON */}
          <MenuItem
            onClick={() => setIsCollapsed(!isCollapsed)}
            icon={isCollapsed ? <MenuOutlinedIcon /> : undefined}
            style={{
              margin: '10px 0 20px 0',
              color: colors.white[100],
            }}
          >
            {!isCollapsed && (
              <Box
                display="flex"
                justifyContent="space-between"
                alignItems="center"
                ml="15px"
              >
                <Typography variant="h3" color={colors.white[100]}>
                  Control panel
                </Typography>
                <IconButton onClick={() => setIsCollapsed(!isCollapsed)}>
                  <MenuOutlinedIcon />
                </IconButton>
              </Box>
            )}
          </MenuItem>

          {!isCollapsed && (
            <Box mb="25px">
              <Box display="flex" justifyContent="center" alignItems="center">
                <img
                  alt="profile-user"
                  width="100px"
                  height="100px"
                  src={plant}
                  style={{ cursor: 'pointer', borderRadius: '50%' }}
                />
              </Box>
              <Box textAlign="center">
                <Typography
                  variant="h2"
                  color={colors.white[100]}
                  fontWeight="bold"
                  sx={{ m: '10px 0 0 0' }}
                >
                  Daniel
                </Typography>
                <Typography variant="h5" color={colors.yellowAccent[500]}>
                  Admin
                </Typography>
              </Box>
            </Box>
          )}

          <Box paddingLeft={isCollapsed ? undefined : '10%'}>
            <Item
              title="Dashboard"
              to="/"
              icon={<HomeOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />

            <Typography
              variant="h6"
              color={colors.white[300]}
              sx={{ m: '15px 0 5px 20px' }}
            >
              Control
            </Typography>
            <Item
              title="All Lamps"
              to="/allLamps"
              icon={<WorkspacesOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <FormControl fullWidth variant="outlined" sx={{ mt: 2, mb: 4 }}>
              <Select
                value={selected}
                onChange={(e) => setSelected(e.target.value)}
              >
                <MuiMenuItem value="group1">
                  <Link to="/group1">Group 1</Link>
                </MuiMenuItem>
                <MuiMenuItem value="group2">
                  <Link to="/group2">Group2</Link>
                </MuiMenuItem>
                <MuiMenuItem value="group3">
                  <Link to="/group3">Group 3</Link>
                </MuiMenuItem>
                <MuiMenuItem value="group4">
                  <Link to="/group4">Group 4</Link>
                </MuiMenuItem>
              </Select>
            </FormControl>

            {/* <Item
              title="Groups "
              to="/groups"
              icon={<ContactsOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            /> */}

            <Typography
              variant="h6"
              color={colors.white[300]}
              sx={{ m: '15px 0 5px 20px' }}
            >
              Data
            </Typography>
            <Item
              title="DLI chart"
              to="/dlichart"
              icon={<LightModeOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <Item
              title="Humidity chart"
              to="/humiditychart"
              icon={<WaterDropOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />

            <Item
              title="System Health"
              to="/systemhealth"
              icon={<PieChartOutlineOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <Item
              title="Diode Usage"
              to="/diodeusage"
              icon={<DataUsageOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <Item
              title="Energy Usage"
              to="/energyusage"
              icon={<EnergySavingsLeafOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <Typography
              variant="h6"
              color={colors.white[300]}
              sx={{ m: '15px 0 5px 20px' }}
            >
              Settings
            </Typography>
            <Item
              title="Light Profiles"
              to="/lightprofiles"
              icon={<CreateNewFolderOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <Item
              title="LED Profiles"
              to="/ledprofiles"
              icon={<TipsAndUpdatesOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
            <Item
              title="Add Group"
              to="/addgroup"
              icon={<FolderSpecialOutlinedIcon />}
              selected={selected}
              setSelected={setSelected}
            />
          </Box>
        </Menu>
      </ProSidebar>
    </Box>
  );
};

export default Sidebar;
