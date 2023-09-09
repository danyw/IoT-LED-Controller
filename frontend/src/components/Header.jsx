import { Typography, Box, useTheme } from "@mui/material";
import { tokens } from "../config/theme";

const Header = ({ title, subtitle }) => {
  const theme = useTheme();
  const colors = tokens(theme.palette.mode);
  return (
    <Box mb="30px">
      <Typography
        variant="h2"
        fontWeight="bold"
        sx={{ m: "0 0 5px 0" }}
        color={colors.white[300]}
      >
        {title}
      </Typography>
      <Typography variant="h5" color={colors.yellowAccent[400]}>
        {subtitle}
      </Typography>
    </Box>
  );
};

export default Header;