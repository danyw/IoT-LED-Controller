const ledProfiles = {
    "Warm White": {
        model: "CREE Xpe-g",
        color: "White",
        wattage: 1,
        spectrum: [
          { wavelength: 380, intensity: 0 },
          { wavelength: 410, intensity: 0 },
          { wavelength: 450, intensity: 10 },
          { wavelength: 480, intensity: 5 },
          { wavelength: 560, intensity: 100 },
          { wavelength: 700, intensity: 5 },
          { wavelength: 720, intensity: 0 },
          { wavelength: 780, intensity: 0 },
          
        ]
      },
      "Cool White": {
        model: "Generic Cool White",
        color: "White",
        wattage: 1,
        spectrum: [
          { wavelength: 380, intensity: 0 },
          { wavelength: 410, intensity: 10 },
          { wavelength: 450, intensity: 100 },
          { wavelength: 500, intensity: 40 },
          { wavelength: 550, intensity: 60 },
          { wavelength: 600, intensity: 40 },
          { wavelength: 650, intensity: 20 },
          { wavelength: 700, intensity: 5 },
          { wavelength: 780, intensity: 0 },
        ]
      },
      "Red": {
        model: "Generic Red",
        color: "Red",
        wattage: 1,
        spectrum: [
          { wavelength: 380, intensity: 0 },
          { wavelength: 600, intensity: 1 },
          { wavelength: 640, intensity: 100 },
          { wavelength: 680, intensity: 1 },
          { wavelength: 780, intensity: 0 },
        ]
      },
      "Far Red": {
        model: "Generic Far Red",
        color: "Far Red",
        wattage: 1,
        spectrum: [
          { wavelength: 380, intensity: 0 },
          { wavelength: 700, intensity: 5 },
          { wavelength: 730, intensity: 100 },
          { wavelength: 760, intensity: 5 },
          { wavelength: 780, intensity: 0 },
        ]
      },
      "Blue": {
        model: "Generic Blue",
        color: "Blue",
        wattage: 1,
        spectrum: [
          { wavelength: 380, intensity: 0 },
          { wavelength: 430, intensity: 10 },
          { wavelength: 470, intensity: 100 },
          { wavelength: 500, intensity: 10 },
          { wavelength: 780, intensity: 0 },

        ]
      },
      "Green": {
        model: "Generic Green",
        color: "Green",
        wattage: 1,
        spectrum: [
          { wavelength: 380, intensity: 0 },
          { wavelength: 510, intensity: 10 },
          { wavelength: 570, intensity: 100 },
          { wavelength: 610, intensity: 10 },
          { wavelength: 780, intensity: 0 },

        ]
      },


}
export default ledProfiles;
  