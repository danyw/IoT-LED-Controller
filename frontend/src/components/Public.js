import React from 'react';
import { Typography, Container, Box, Button, Link as MuiLink } from '@mui/material';
import { Link } from 'react-router-dom';

const Public = () => {
  return (
    <Container maxWidth="sm">
      <Box sx={{ mt: 4, mb: 4, p: 3, backgroundColor: 'primary.main', borderRadius: 2, boxShadow: 3 }}>
        <Typography variant="h4" gutterBottom sx={{ color: 'white.main' }}>
          Welcome to <span style={{ whiteSpace: 'nowrap' }}>IoT Light Controller App!</span>
        </Typography>
        <Typography variant="body1" gutterBottom sx={{ color: 'neutral.main' }}>
          This App is used to control the network of lamps.
        </Typography>
        <Typography variant="body1" gutterBottom sx={{ color: 'neutral.main' }}>
          Designed by: Daniel
        </Typography>
        <Typography variant="body1" gutterBottom sx={{ color: 'neutral.main' }}>
          Login for testing:
        </Typography>
        <Typography variant="body1" gutterBottom sx={{ color: 'neutral.main' }}>
          Username: admin
        </Typography>
        <Typography variant="body1" gutterBottom sx={{ color: 'neutral.main' }}>
          Password: admin1234
        </Typography>
        <MuiLink component={Link} to="/login" sx={{ textDecoration: 'none' }}>
          <Button variant="contained" sx={{ mt: 2, backgroundColor: 'redAccent.main' }}>
            Employee Login
          </Button>
        </MuiLink>
      </Box>
    </Container>
  );
};

export default Public;
