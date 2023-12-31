import { Outlet } from 'react-router-dom';
import { useState } from 'react';
import Topbar from './Topbar';
import Sidebar from './Sidebar';

const DashLayout = () => {
  const [isSidebar, setIsSidebar] = useState(true);
  return (
    <div className="app">
      <Sidebar isSidebar={isSidebar} />
      <main className="main">
        <Topbar setIsSidebar={setIsSidebar} />
        <Outlet />
      </main>
    </div>
  );
};

export default DashLayout;
