import { Box } from "@mui/material";
// import Header from "../components/Header";
import LedProfileChart from "../../components/charts/LedProfileChart";

const LEDProfiles = () => {
    const defaultProfileName = "Cool White";
  return (
    <Box m="20px">
      {/* <Header title="Line Chart" subtitle="Simple Line Chart" /> */}
      <Box height="75vh">
        <LedProfileChart profileName={defaultProfileName}/>
      </Box>
    </Box>
  );
};

export default LEDProfiles;