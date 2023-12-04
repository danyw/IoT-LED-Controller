import { useState } from 'react';
import { useMode } from './hooks/useMode';
import { ColorModeContext } from './context/ColorModeContext';
import { Route, Routes } from 'react-router-dom';
import { CssBaseline, ThemeProvider } from '@mui/material';
import Sidebar from './components/Sidebar';
import Topbar from './components/Topbar';
import Lamps from './components/Lamps';
// import Line from './Views/line';


function App() {
  const [isSidebar, setIsSidebar] = useState(true);
  const [theme, colorMode] = useMode();

  return (
    <ColorModeContext.Provider value={colorMode}>
      <ThemeProvider theme={theme}>
        <CssBaseline />
        <div className="app">
          <Sidebar isSidebar={isSidebar} />
          <main className="main">
            <Topbar setIsSidebar={setIsSidebar} />
<Lamps />
{/* <Line /> */}
            <Routes>
              {/* <Route path="/" element={<Dashboard />} /> */}
              {/* <Route path="/lamps" element={<Lamps />} /> */}
              

            </Routes>
          </main>
        </div>
      </ThemeProvider>
    </ColorModeContext.Provider>
  );
}

export default App;
