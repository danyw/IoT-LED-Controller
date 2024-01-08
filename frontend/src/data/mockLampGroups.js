const lampGroups = {
  'Room 1': {
    groupName: 'Seedling Stage Group',
    room: 'Room 1',
    purpose: 'Germination and early growth',
    lamps: [
      {
        id: 'lamp1',
        wattage: 120,
        spectrum: 'Full Spectrum',
        isOn: true,
        intensity: '75%',
        operatingHours: { start: '06:00', end: '18:00' },
        ledsUsed: {warmWhite: {W: 3, Qty: 10}, coolWhite: {W: 3, Qty: 10}, red: {W: 3, Qty: 5}, green: {W: 3, Qty: 3}, blue: {W: 3, Qty: 7}  }
      },
    ],
    controls: {
      currentProgram: 'Early Growth Schedule',
    },
  },
  'Room 2': {
    groupName: 'Vegetative Stage Group',
    room: 'Room 2',
    purpose: 'Support vegetative growth',
    lamps: [
      {
        id: 'lamp2',
        type: 'LED',
        wattage: 30,
        spectrum: 'Blue-Enriched',
        isOn: false,
        intensity: '85%',
        operatingHours: { start: '07:00', end: '19:00' },
        ledsUsed: {warmWhite: {W: 3, Qty: 10}, coolWhite: {W: 3, Qty: 10}, red: {W: 3, Qty: 5}, green: {W: 3, Qty: 3}, blue: {W: 3, Qty: 7}  }

      },
    ],
    controls: {
      currentProgram: 'Vegetative Growth Schedule',
    },
  },
};

export default lampGroups;