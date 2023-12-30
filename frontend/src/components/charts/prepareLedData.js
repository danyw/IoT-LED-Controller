export const prepareLedData = (ledSpecifications) => {
    // ledSpecifications is an array of objects: { model: "CRI XPe", color: "Red", wattage: 3W, minWavelength: x, peakWavelength: y, maxWavelength: z }
    return ledSpecifications.map(ledSpec => ({
      id: ledSpec.model,
      color: ledSpec.color,
      data: ledSpec.spectrum.map(point => ({
        x: point.wavelength,
        y: point.intensity
      }))
    }));
  };
  
