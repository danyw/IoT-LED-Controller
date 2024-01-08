export const transformProfileToSchedule = (profileData) => {

  if (!profileData) {
    console.error('Invalid profile data:', profileData);
    return [];
  }

  const schedule = [];
  const colors = Object.keys(profileData);
  const hoursInDay = 24;

  // Iterate over each hour
  for (let hour = 0; hour < hoursInDay; hour++) {
    // For each color, get the value at the current hour
    const hourValues = colors.map(color => profileData[color][hour]);
    // Add the values for all colors at this hour to the schedule
    schedule.push(hourValues);
  }

  return schedule; // This will be an array of 24 arrays, each containing the values for each color at that hour
};
