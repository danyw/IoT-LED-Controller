import { useMode } from './hooks/useMode';
import { ColorModeContext } from './context/ColorModeContext';
import { Route, Routes } from 'react-router-dom';
import { CssBaseline, ThemeProvider } from '@mui/material';
import Layout from './components/Layout';
import DashLayout from './components/DashLayout';
import LedProfiles from './features/led profiles/LedProfiles';
import Dashboard from './features/dashboard/Dashboard';
import AllLamps from './features/lamp groups/AllLamps';
import LampDetails from './features/lamp groups/LampDetails';

function App() {
  const [theme, colorMode] = useMode();

  return (
    <ColorModeContext.Provider value={colorMode}>
      <ThemeProvider theme={theme}>
        <CssBaseline />

        <Routes>
          <Route path="/" element={<Layout />}>
            {/* Public routes */}
            {/* Private routes */}
            <Route path="dash" element={<DashLayout />}>
              <Route index element={<Dashboard />} />
              {/* <Route path="/lamps" element={<Lamps />} /> */}
              <Route path="alllamps">
                <Route index element={<AllLamps />} />
              </Route>
              <Route path="lampdetails">
                <Route index element={<LampDetails />} />
              </Route>
              <Route path="ledprofiles">
                <Route index element={<LedProfiles />} />
              </Route>

              {/* <Route path="lampgroupslist">
                <Route index element={<LampGroupsList />} />
                <Route path=":id" element={<LampGroup />} />
                <Route path="newgroup" element={<NewLampGroup />} />
              </Route> */}
            </Route>
          </Route>
        </Routes>
      </ThemeProvider>
    </ColorModeContext.Provider>
  );
}

export default App;
