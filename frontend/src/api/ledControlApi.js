export const sendLedControlRequest = async (color, value) => {
    try {
      const response = await fetch('http://192.168.0.108:5001/mqtt/ledControl', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          color: color.replace(/\s+/g, '').toLowerCase(), // Converts 'warm white' to 'warmwhite'
        // color: 'warmwhite',
          value: value
        })
      });
  
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
  
      // Handle response here, if needed
    } catch (error) {
      console.error('Error sending LED control request:', error);
    }
  };
  

